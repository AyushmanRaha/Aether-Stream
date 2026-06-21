#include <aether/core/status.hpp>

namespace aether {

std::string_view status_code_name(StatusCode code) noexcept {
    switch (code) {
    case StatusCode::ok:
        return "ok";
    case StatusCode::unknown:
        return "unknown";
    case StatusCode::full:
        return "full";
    case StatusCode::empty:
        return "empty";
    case StatusCode::invalid_argument:
        return "invalid_argument";
    case StatusCode::io_error:
        return "io_error";
    case StatusCode::corrupted_record:
        return "corrupted_record";
    case StatusCode::out_of_space:
        return "out_of_space";
    }

    return "unknown";
}

std::string_view status_code_message(StatusCode code) noexcept {
    switch (code) {
    case StatusCode::ok:
        return "operation completed successfully";
    case StatusCode::unknown:
        return "unknown error";
    case StatusCode::full:
        return "resource is full";
    case StatusCode::empty:
        return "resource is empty";
    case StatusCode::invalid_argument:
        return "invalid argument";
    case StatusCode::io_error:
        return "I/O error";
    case StatusCode::corrupted_record:
        return "corrupted record";
    case StatusCode::out_of_space:
        return "not enough space";
    }

    return "unknown error";
}

} // namespace aether
