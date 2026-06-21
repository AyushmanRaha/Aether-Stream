#include <aether/core/expected.hpp>
#include <aether/core/status.hpp>
#include <iostream>
#include <string_view>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }

    return 0;
}

} // namespace

int main() {
    int failures = 0;

    const aether::Status default_status;
    const aether::Status ok_status = aether::Status::ok();
    const aether::Status full_status{aether::StatusCode::full};
    const aether::Status custom_status{aether::StatusCode::io_error, "custom detail"};
    const aether::Status another_full_status{aether::StatusCode::full};
    const aether::Expected<int> value_result{42};
    const aether::Expected<int> error_result{aether::Status{aether::StatusCode::empty}};
    const aether::Expected<void> void_success;
    const aether::Expected<void> void_error{aether::Status{aether::StatusCode::out_of_space}};

    failures += check(default_status.is_ok(), "default Status should be OK");
    failures += check(ok_status.is_ok(), "Status::ok should be OK");
    failures += check(!full_status.is_ok(), "full status should not be OK");
    failures +=
        check(aether::status_code_name(aether::StatusCode::full) == std::string_view{"full"},
              "full status name should be full");
    failures += check(!aether::status_code_message(aether::StatusCode::empty).empty(),
                      "empty status message should not be empty");
    failures += check(custom_status.message() == std::string_view{"custom detail"},
                      "custom detail should override default message");
    failures += check(full_status == another_full_status, "equal statuses should compare equal");
    failures += check(full_status != custom_status, "different statuses should compare unequal");
    failures += check(value_result.has_value(), "Expected<int> value should have value");
    failures += check(value_result.value() == 42, "Expected<int> value should be 42");
    failures += check(!error_result.has_value(), "Expected<int> error should not have value");
    failures += check(error_result.status().code() == aether::StatusCode::empty,
                      "Expected<int> error should expose status");
    failures += check(void_success.has_value(), "Expected<void> default should be success");
    failures += check(!void_error.has_value(), "Expected<void> error should not have value");
    failures += check(void_error.status().code() == aether::StatusCode::out_of_space,
                      "Expected<void> error should expose status");

    if (failures != 0) {
        return 1;
    }

    std::cout << "status test passed\n";
    return 0;
}
