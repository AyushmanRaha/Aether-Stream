#include <aether/version.hpp>
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

    failures += check(aether::version_major == 0, "version_major should be 0");
    failures += check(aether::version_minor == 1, "version_minor should be 1");
    failures += check(aether::version_patch == 0, "version_patch should be 0");
    failures += check(aether::version_string() == std::string_view{"0.1.0"},
                      "version_string should be 0.1.0");
    failures += check(!aether::version_string().empty(), "version_string should not be empty");

    if (failures != 0) {
        return 1;
    }

    std::cout << "version test passed\n";
    return 0;
}
