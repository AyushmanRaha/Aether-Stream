// Minimal smoke test: prints the built library version to confirm linking works.
#include <aether/version.hpp>
#include <iostream>

int main() {
    std::cout << "Aether-Stream " << aether::version_string() << '\n';
    return 0;
}
