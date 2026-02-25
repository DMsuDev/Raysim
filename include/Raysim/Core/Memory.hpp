#pragma once

// Memory.hpp - Smart pointer aliases, factory functions, and type casting utilities
// Provides convenient type aliases, factory functions, and safe casting helpers for memory management

#include <memory>

// ============================================================================
// SMART POINTER ALIASES
// ============================================================================

// Convenience alias for unique pointers (stack-like ownership)
template<typename T>
using Scope = std::unique_ptr<T>;

// Convenience alias for shared pointers (reference-counted ownership)
template<typename T>
using Shared = std::shared_ptr<T>;

namespace RS {

// ============================================================================
// CASTING UTILITIES
// ============================================================================

// Safe static cast - compile-time type checking for derived/base conversions
// Usage: auto derived = Cast<DerivedType*>(basePtr);
//        int value = Cast<int>(floatValue);
template<typename To, typename From>
[[nodiscard]] constexpr To Cast(From value) {
    return static_cast<To>(value);
}

// Const cast removal - explicitly cast away const qualifier
// Usage: auto mutable_ptr = ConstCast<MyClass*>(const_ptr);
// NOTE: Use with caution - modifying const data is undefined behavior
template<typename To, typename From>
[[nodiscard]] constexpr To ConstCast(From value) {
    return const_cast<To>(value);
}

// Pointer reinterpret cast - reinterprets bit pattern (dangerous, use sparingly)
// Usage: auto opaque = BitCast<void*>(fontPtr);
// NOTE: This is a low-level operation used for opaque handles. Prefer static_cast when possible.
template<typename To, typename From>
[[nodiscard]] constexpr To BitCast(From value) {
    return reinterpret_cast<To>(value);
}

#if __cplusplus >= 202002L
// C++20 bit_cast - safe bit pattern reinterpretation for trivially copyable types
// Usage: auto bits = SafeBitCast<uint64_t>(doubleValue);
#include <bit>
template<typename To, typename From>
[[nodiscard]] constexpr To SafeBitCast(const From& value) {
    static_assert(std::is_trivially_copyable_v<From>, "From type must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<To>, "To type must be trivially copyable");
    static_assert(sizeof(From) == sizeof(To), "Types must have the same size");
    return std::bit_cast<To>(value);
}
#endif

// ============================================================================
// SMART POINTER FACTORY FUNCTIONS
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

/*
============================================================================
COMPLETE USAGE EXAMPLES
============================================================================

SMART POINTER USAGE:
    // Unique pointer - exclusive ownership
    auto player = RS::CreateScope<Player>("Alice", 100);
    auto vec = RS::CreateScope<std::vector<int>>();

    // Shared pointer - reference-counted ownership
    auto resource = RS::CreateShared<Texture2D>("path/to/texture.png");

CASTING USAGE:
    // Static cast - safe conversions
    void* raw_ptr = renderer.GetData();
    auto typed_ptr = RS::Cast<FontData*>(raw_ptr);

    // Const cast - remove const (use cautiously)
    const MyObject* const_obj =  ... ;
    auto mutable_obj = RS::ConstCast<MyObject*>(const_obj);

    // Bit cast - low-level reinterpretation
    raylib::Font* font = ... ;
    void* opaque_handle = RS::BitCast<void*>(font);

    // Safe bit cast (C++20) - for trivially copyable types
    #if __cplusplus >= 202002L
    uint64_t bits = RS::SafeBitCast<uint64_t>(3.14159);
    #endif

PRACTICAL EXAMPLE - Font Manager:
    void FontManager::LoadFont(const std::string& path, int size) {
        ::Font* raylib_font = ::LoadFontEx(path.c_str(), size, nullptr, 0);

        // Store as opaque void* to hide raylib dependency
        font_ = RS::BitCast<void*>(raylib_font);

        // Later, retrieve and cast back
        auto* font_ptr = RS::Cast<::Font*>(RS::BitCast<intptr_t>(font_));
        ::DrawTextEx(*font_ptr, "Text", {0, 0}, 32, 0, WHITE);
    }
*/

// NOTES:
// - RS::Cast<T>() = static_cast (safe, preferred for type hierarchy)
// - RS::ConstCast<T>() = const_cast (removes const, use carefully)
// - RS::BitCast<T>() = reinterpret_cast (low-level, for opaque handles)
// - RS::SafeBitCast<T>() = std::bit_cast (C++20, safe bit-pattern reinterpretation)
// - All casting functions use [[nodiscard]] to warn about ignored results
