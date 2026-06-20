#include <aether/version.hpp>
#include <iostream>

int main() {
    std::cout << "Aether-Stream " << aether::version_string() << '\n';
    return 0;
}
