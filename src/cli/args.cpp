#include <aether/cli/args.hpp>
#include <algorithm>
#include <charconv>
#include <limits>
#include <string>
#include <string_view>

namespace aether::cli {
namespace {

constexpr std::size_t supported_capacities[] = {64, 256, 1024, 4096, 65536};
constexpr std::size_t supported_payload_sizes[] = {8, 32, 64, 256, 1024};

[[nodiscard]] Status invalid(std::string_view detail) noexcept {
    return Status{StatusCode::invalid_argument, detail};
}

[[nodiscard]] bool is_supported(std::size_t value, std::span<const std::size_t> supported) {
    return std::find(supported.begin(), supported.end(), value) != supported.end();
}

[[nodiscard]] Expected<std::uint64_t> parse_u64(std::string_view text) noexcept {
    if (text.empty() || text.front() == '-') {
        return invalid("expected a non-negative unsigned integer");
    }
    std::uint64_t value{};
    const char* const begin = text.data();
    const char* const end = text.data() + text.size();
    const auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc{} || result.ptr != end) {
        return invalid("invalid unsigned integer");
    }
    return value;
}

[[nodiscard]] Expected<std::size_t> parse_size(std::string_view text) noexcept {
    auto parsed = parse_u64(text);
    if (!parsed.has_value()) {
        return parsed.status();
    }
    if (parsed.value() > std::numeric_limits<std::size_t>::max()) {
        return invalid("value is too large for size_t");
    }
    return static_cast<std::size_t>(parsed.value());
}

class ArgCursor {
public:
    explicit ArgCursor(std::span<const char* const> args) : args_(args) {}

    [[nodiscard]] std::size_t size() const noexcept {
        return args_.size();
    }
    [[nodiscard]] std::string_view at(std::size_t index) const noexcept {
        return args_[index];
    }

    [[nodiscard]] Expected<std::string_view> value(std::size_t& index,
                                                   std::string_view option) const {
        const std::string_view current = at(index);
        const std::size_t eq = current.find('=');
        if (eq != std::string_view::npos) {
            const std::string_view out = current.substr(eq + 1U);
            if (out.empty()) {
                return invalid("missing option value");
            }
            return out;
        }
        if (index + 1U >= size()) {
            return invalid("missing option value");
        }
        const std::string_view out = at(++index);
        if (out.empty() || out.starts_with('-')) {
            return invalid("missing option value");
        }
        (void)option;
        return out;
    }

private:
    std::span<const char* const> args_;
};

[[nodiscard]] bool matches(std::string_view arg, std::string_view option) noexcept {
    return arg == option || arg.starts_with(std::string(option) + "=");
}

} // namespace

std::string_view bench_help() noexcept {
    return "Usage: aether_bench [options]\n"
           "CLI demo benchmark for local Broker throughput. Not official performance data.\n\n"
           "Options:\n  -h, --help\n  --messages <N>\n  --payload-size <8|32|64|256|1024>\n"
           "  --capacity <64|256|1024|4096|65536>\n  --output-dir <path>\n";
}
std::string_view pub_help() noexcept {
    return "Usage: aether_pub [options]\n"
           "Write generated OrderEvent records to a local WAL-backed broker demo.\n\n"
           "Options:\n  -h, --help\n  --wal <path>\n  --messages <N>\n"
           "  --wal-size <bytes>\n  --flush-on-commit\n";
}
std::string_view sub_help() noexcept {
    return "Usage: aether_sub [options]\n"
           "Local demo subscriber only: no networking and no remote/live broker subscription.\n"
           "Without --wal it publishes then consumes an in-process Broker. With --wal it replays "
           "OrderEvent records written by aether_pub.\n\n"
           "Options:\n  -h, --help\n  --messages <N>\n  --capacity <64|256|1024|4096|65536>\n"
           "  --limit <N>\n  --wal <path>\n";
}
std::string_view replay_help() noexcept {
    return "Usage: aether_replay [options]\n"
           "Generic raw WAL replay with safe payload previews.\n\n"
           "Options:\n  -h, --help\n  --wal <path>\n  --limit <N>\n"
           "  --payload-preview-bytes <N>\n";
}
std::string_view inspect_wal_help() noexcept {
    return "Usage: aether_inspect_wal [options]\n"
           "Inspect and summarize a WAL file.\n\n"
           "Options:\n  -h, --help\n  --wal <path>\n  --limit <N>\n  --verbose\n";
}

Expected<BenchOptions> parse_bench_args(std::span<const char* const> args) {
    BenchOptions options{};
    ArgCursor cursor{args};
    for (std::size_t i = 1; i < cursor.size(); ++i) {
        const std::string_view arg = cursor.at(i);
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            continue;
        }
        if (matches(arg, "--messages")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.messages = n.value();
            continue;
        }
        if (matches(arg, "--payload-size")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_size(v.value());
            if (!n)
                return n.status();
            if (!is_supported(n.value(), supported_payload_sizes))
                return invalid("unsupported payload size");
            options.payload_size = n.value();
            continue;
        }
        if (matches(arg, "--capacity")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_size(v.value());
            if (!n)
                return n.status();
            if (!is_supported(n.value(), supported_capacities))
                return invalid("unsupported capacity");
            options.capacity = n.value();
            continue;
        }
        if (matches(arg, "--output-dir")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            options.output_dir = std::filesystem::path{v.value()};
            continue;
        }
        return invalid("unknown option");
    }
    return options;
}

Expected<PubOptions> parse_pub_args(std::span<const char* const> args) {
    PubOptions options{};
    ArgCursor cursor{args};
    for (std::size_t i = 1; i < cursor.size(); ++i) {
        const std::string_view arg = cursor.at(i);
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            continue;
        }
        if (arg == "--flush-on-commit") {
            options.flush_on_commit = true;
            continue;
        }
        if (matches(arg, "--wal")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            options.wal_path = std::filesystem::path{v.value()};
            continue;
        }
        if (matches(arg, "--messages")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.messages = n.value();
            continue;
        }
        if (matches(arg, "--wal-size")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.wal_size_bytes = n.value();
            continue;
        }
        return invalid("unknown option");
    }
    return options;
}

Expected<SubOptions> parse_sub_args(std::span<const char* const> args) {
    SubOptions options{};
    ArgCursor cursor{args};
    for (std::size_t i = 1; i < cursor.size(); ++i) {
        const std::string_view arg = cursor.at(i);
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            continue;
        }
        if (matches(arg, "--wal")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            options.wal_path = std::filesystem::path{v.value()};
            continue;
        }
        if (matches(arg, "--messages")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.messages = n.value();
            continue;
        }
        if (matches(arg, "--limit")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.limit = n.value();
            continue;
        }
        if (matches(arg, "--capacity")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_size(v.value());
            if (!n)
                return n.status();
            if (!is_supported(n.value(), supported_capacities))
                return invalid("unsupported capacity");
            options.capacity = n.value();
            continue;
        }
        return invalid("unknown option");
    }
    return options;
}

Expected<ReplayOptions> parse_replay_args(std::span<const char* const> args) {
    ReplayOptions options{};
    ArgCursor cursor{args};
    for (std::size_t i = 1; i < cursor.size(); ++i) {
        const std::string_view arg = cursor.at(i);
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            continue;
        }
        if (matches(arg, "--wal")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            options.wal_path = std::filesystem::path{v.value()};
            continue;
        }
        if (matches(arg, "--limit")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.limit = n.value();
            continue;
        }
        if (matches(arg, "--payload-preview-bytes")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_size(v.value());
            if (!n)
                return n.status();
            options.payload_preview_bytes = n.value();
            continue;
        }
        return invalid("unknown option");
    }
    return options;
}

Expected<InspectWalOptions> parse_inspect_wal_args(std::span<const char* const> args) {
    InspectWalOptions options{};
    ArgCursor cursor{args};
    for (std::size_t i = 1; i < cursor.size(); ++i) {
        const std::string_view arg = cursor.at(i);
        if (arg == "--help" || arg == "-h") {
            options.help = true;
            continue;
        }
        if (arg == "--verbose") {
            options.verbose = true;
            continue;
        }
        if (matches(arg, "--wal")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            options.wal_path = std::filesystem::path{v.value()};
            continue;
        }
        if (matches(arg, "--limit")) {
            auto v = cursor.value(i, arg);
            if (!v)
                return v.status();
            auto n = parse_u64(v.value());
            if (!n)
                return n.status();
            options.limit = n.value();
            continue;
        }
        return invalid("unknown option");
    }
    return options;
}

} // namespace aether::cli
