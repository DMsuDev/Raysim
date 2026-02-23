#pragma once

// Memory.hpp - Smart pointer aliases and factory functions
// Provides convenient type aliases and factory functions for memory management

#include <memory>

// ============================================================================
// SMART POINTER ALIASES AND FACTORY FUNCTIONS
// ============================================================================

// Convenience alias for unique pointers (stack-like ownership)
template<typename T>
using Scope = std::unique_ptr<T>;

// Convenience alias for shared pointers (reference-counted ownership)
template<typename T>
using Shared = std::shared_ptr<T>;

namespace RS {

// ============================================================================
// UTILITY TEMPLATES
// ============================================================================

// NOTE: [[nodiscard]] is used to indicate that the return value should not be ignored
// EXAMPLE: auto myObj = CreateScope<MyClass>(arg1, arg2); // Correct usage
//          CreateScope<MyClass>(arg1, arg2); // Warning: return value ignored

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

} // namespace RS
