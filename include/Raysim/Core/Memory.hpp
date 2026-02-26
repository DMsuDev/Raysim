#pragma once

#include <memory>

namespace RS {

// Convenience alias for unique pointers (stack-like ownership)
template<typename T>
using Scope = std::unique_ptr<T>;

// Convenience alias for shared pointers (reference-counted ownership)
template<typename T>
using Shared = std::shared_ptr<T>;

// Factory function: Creates a unique_ptr with perfect forwarding
// Usage: auto myObj = CreateScope<MyClass>(arg1, arg2);
template<typename T, typename... Args>
[[nodiscard]] constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Factory function: Creates a shared_ptr with perfect forwarding
// Usage: auto myObj = CreateShared<MyClass>(arg1, arg2);
template<typename T, typename... Args>
[[nodiscard]] constexpr Shared<T> CreateShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#if __cplusplus >= 202002L
// C++20 bit_cast - safe bit pattern reinterpretation for trivially copyable types
// Usage: auto bits = SafeBitCast<uint64_t>(doubleValue);
#include <bit>
template<typename To, typename From>
[[nodiscard]] constexpr To SafeBitCast(const From& value)
{
    static_assert(std::is_trivially_copyable_v<From>, "From type must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<To>, "To type must be trivially copyable");
    static_assert(sizeof(From) == sizeof(To), "Types must have the same size");

    return std::bit_cast<To>(value);
}
#endif

} // namespace RS

// NOTE: [[nodiscard]] is used to indicate that the return value should not be ignored
// EXAMPLE: auto myObj = CreateScope<MyClass>(arg1, arg2); // Correct usage
//          CreateScope<MyClass>(arg1, arg2); // Warning: return value ignored
