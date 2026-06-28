#include <aether/zero_copy_spsc.hpp>
#include <cstdint>
#include <iostream>
#include <memory>

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
    aether::ZeroCopySpsc<int, 4> queue;
    failures += check(queue.empty() && !queue.full(), "new zero-copy queue empty and not full");

    auto reservation = queue.try_reserve();
    failures += check(reservation.has_value(), "reserve succeeds");
    reservation->construct(42);
    failures += check(reservation->constructed(), "reservation reports constructed");
    failures += check(reservation->commit(), "commit succeeds");
    int out = 0;
    failures += check(queue.try_consume(out) && out == 42, "consume committed value");

    auto no_construct = queue.try_reserve();
    failures += check(no_construct.has_value(), "reserve without construct succeeds");
    failures += check(!no_construct->commit(), "commit without construct returns false");
    failures += check(queue.empty(), "commit without construct publishes nothing");
    no_construct->cancel();

    auto cancelled = queue.try_reserve();
    cancelled->construct(7);
    cancelled->cancel();
    failures += check(queue.empty(), "cancel prevents visibility");

    {
        auto auto_cancel = queue.try_reserve();
        auto_cancel->construct(8);
    }
    failures += check(queue.empty(), "reservation destructor cancels automatically");

    auto active = queue.try_reserve();
    failures += check(active.has_value(), "active reservation obtained");
    failures += check(!queue.try_reserve().has_value(), "second reserve fails while active");
    active->cancel();

    aether::ZeroCopySpsc<std::uint64_t, 1024> ordered;
    for (std::uint64_t i = 0; i < 1000; ++i) {
        auto r = ordered.try_reserve();
        failures += check(r.has_value(), "ordered reserve succeeds");
        r->construct(i);
        failures += check(r->commit(), "ordered commit succeeds");
    }
    for (std::uint64_t i = 0; i < 1000; ++i) {
        std::uint64_t actual = 9999;
        failures += check(ordered.try_consume(actual) && actual == i, "FIFO order preserved");
    }

    aether::ZeroCopySpsc<int, 4> wrap;
    for (int cycle = 0; cycle < 200; ++cycle) {
        auto r = wrap.try_reserve();
        r->construct(cycle);
        r->commit();
        int actual = -1;
        failures += check(wrap.try_consume(actual) && actual == cycle, "wraparound cycle succeeds");
    }

    aether::ZeroCopySpsc<int, 4> full;
    for (int i = 0; i < 4; ++i) {
        auto r = full.try_reserve();
        r->construct(i);
        r->commit();
    }
    failures += check(full.full(), "queue reports full");
    failures += check(!full.try_reserve().has_value(), "reserve fails when full");

    aether::ZeroCopySpsc<std::unique_ptr<int>, 4> move_queue;
    auto move_res = move_queue.try_reserve();
    move_res->construct(std::make_unique<int>(123));
    move_res->commit();
    std::unique_ptr<int> moved;
    failures += check(move_queue.try_consume(moved) && moved && *moved == 123,
                      "move-only payload consumes");

    if (failures != 0) {
        return 1;
    }
    std::cout << "zero-copy SPSC test passed\n";
    return 0;
}
