#pragma once

#include <cstdint>
#include <string_view>

namespace aether {

enum class StatusCode : std::uint8_t {
    ok = 0,
    unknown,
    full,
    empty,
    invalid_argument,
    io_error,
    corrupted_record,
    out_of_space
};

[[nodiscard]] std::string_view status_code_name(StatusCode code) noexcept;
[[nodiscard]] std::string_view status_code_message(StatusCode code) noexcept;

class Status {
public:
    constexpr Status() noexcept = default;
    constexpr explicit Status(StatusCode code, std::string_view detail = {}) noexcept
        : code_(code), detail_(detail) {}

    [[nodiscard]] static constexpr Status ok() noexcept {
        return Status{};
    }

    [[nodiscard]] static constexpr Status error(StatusCode code,
                                                std::string_view detail = {}) noexcept {
        return Status{code, detail};
    }

    [[nodiscard]] constexpr bool is_ok() const noexcept {
        return code_ == StatusCode::ok;
    }
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return is_ok();
    }
    [[nodiscard]] constexpr StatusCode code() const noexcept {
        return code_;
    }
    [[nodiscard]] constexpr std::string_view detail() const noexcept {
        return detail_;
    }

    [[nodiscard]] std::string_view message() const noexcept {
        if (!detail_.empty()) {
            return detail_;
        }

        return status_code_message(code_);
    }

private:
    StatusCode code_{StatusCode::ok};
    std::string_view detail_{};
};

[[nodiscard]] constexpr bool operator==(Status lhs, Status rhs) noexcept {
    return lhs.code() == rhs.code() && lhs.detail() == rhs.detail();
}

[[nodiscard]] constexpr bool operator!=(Status lhs, Status rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace aether
