#include <aether/spsc_ring_buffer.hpp>
#include <iostream>
#include <memory>
#include <type_traits>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}

struct MoveOnlyPayload {
    explicit MoveOnlyPayload(int input) : value{input} {}
    MoveOnlyPayload(const MoveOnlyPayload&) = delete;
    MoveOnlyPayload& operator=(const MoveOnlyPayload&) = delete;
    MoveOnlyPayload(MoveOnlyPayload&& other) noexcept : value{other.value} {
        other.value = -1;
    }
    MoveOnlyPayload& operator=(MoveOnlyPayload&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
        }
        return *this;
    }
    int value;
};

static_assert(!std::is_copy_constructible_v<MoveOnlyPayload>);
static_assert(std::is_move_constructible_v<MoveOnlyPayload>);
static_assert(std::is_move_assignable_v<MoveOnlyPayload>);
static_assert(!std::is_default_constructible_v<MoveOnlyPayload>);

} // namespace

int main() {
    int failures = 0;

    aether::SpscRingBuffer<std::unique_ptr<int>, 4> ptr_queue;
    auto ptr = std::make_unique<int>(42);
    failures += check(ptr_queue.try_push(std::move(ptr)), "unique_ptr rvalue push should succeed");
    failures += check(ptr == nullptr, "unique_ptr should be moved from after push");

    std::unique_ptr<int> popped;
    failures += check(ptr_queue.try_pop(popped), "unique_ptr pop should succeed");
    failures += check(popped != nullptr && *popped == 42, "unique_ptr popped value should match");

    for (int value = 0; value < 4; ++value) {
        failures += check(ptr_queue.try_push(std::make_unique<int>(value)),
                          "unique_ptr fill push should succeed");
    }
    failures +=
        check(!ptr_queue.try_push(std::make_unique<int>(99)), "unique_ptr full push should fail");
    for (int expected = 0; expected < 4; ++expected) {
        popped.reset();
        failures += check(ptr_queue.try_pop(popped), "unique_ptr drain pop should succeed");
        failures += check(popped != nullptr && *popped == expected,
                          "unique_ptr FIFO order should be preserved");
    }

    aether::SpscRingBuffer<MoveOnlyPayload, 4> payload_queue;
    failures += check(payload_queue.try_emplace(7), "move-only emplace should succeed");
    failures +=
        check(payload_queue.try_push(MoveOnlyPayload{8}), "move-only rvalue push should succeed");
    MoveOnlyPayload out{0};
    failures += check(payload_queue.try_pop(out), "move-only first pop should succeed");
    failures += check(out.value == 7, "move-only first value should match");
    failures += check(payload_queue.try_pop(out), "move-only second pop should succeed");
    failures += check(out.value == 8, "move-only second value should match");

    for (int value = 10; value < 14; ++value) {
        failures +=
            check(payload_queue.try_emplace(value), "move-only fill emplace should succeed");
    }
    failures += check(!payload_queue.try_emplace(14), "move-only full emplace should fail");
    for (int expected = 10; expected < 14; ++expected) {
        failures += check(payload_queue.try_pop(out), "move-only drain pop should succeed");
        failures += check(out.value == expected, "move-only FIFO order should be preserved");
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "spsc move-only test passed\n";
    return 0;
}
