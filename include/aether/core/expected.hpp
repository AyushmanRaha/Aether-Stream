#pragma once

#include <aether/core/status.hpp>
#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

namespace aether {

namespace detail {

[[nodiscard]] constexpr Status normalize_expected_error(Status status) noexcept {
    if (status.is_ok()) {
        return Status{StatusCode::invalid_argument, "Expected error status must not be ok"};
    }

    return status;
}

} // namespace detail

template <typename T> class Expected {
    static_assert(!std::is_void_v<T>, "Expected<void> is provided as a specialization");

public:
    Expected(const T& value) : storage_(value) {}
    Expected(T&& value) : storage_(std::move(value)) {}
    Expected(Status status) : storage_(detail::normalize_expected_error(status)) {}

    [[nodiscard]] bool has_value() const noexcept {
        return std::holds_alternative<T>(storage_);
    }
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }

    T& value() & noexcept {
        T* value = std::get_if<T>(&storage_);
        assert(value != nullptr);
        return *value;
    }

    const T& value() const& noexcept {
        const T* value = std::get_if<T>(&storage_);
        assert(value != nullptr);
        return *value;
    }

    T&& value() && noexcept {
        T* value = std::get_if<T>(&storage_);
        assert(value != nullptr);
        return std::move(*value);
    }

    [[nodiscard]] Status status() const noexcept {
        if (has_value()) {
            return Status::ok();
        }

        return std::get<Status>(storage_);
    }

private:
    std::variant<T, Status> storage_;
};

template <> class Expected<void> {
public:
    constexpr Expected() noexcept = default;
    constexpr Expected(Status status) noexcept : status_(detail::normalize_expected_error(status)) {
        if (status.is_ok()) {
            status_ = Status::ok();
        }
    }

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return status_.is_ok();
    }
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return has_value();
    }
    [[nodiscard]] constexpr Status status() const noexcept {
        return status_;
    }

private:
    Status status_{};
};

} // namespace aether
