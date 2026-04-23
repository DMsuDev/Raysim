#pragma once

#include "Raysim/Core/PlatformDetection.hpp"

#include <memory>

// Define platform-specific debug break
// In debug builds, this will trigger a breakpoint in the debugger
// when an assertion fails.
#ifdef RS_BUILD_DEBUG
	#ifdef RS_PLATFORM_WINDOWS
		#define RS_DEBUGBREAK() __debugbreak()
	#elif defined(RS_PLATFORM_LINUX)
		#include <signal.h>
        #define RS_DEBUGBREAK() __builtin_trap()
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	// Enable assertions only in debug builds
    #define RS_ENABLE_ASSERTS
#else
	#define RS_DEBUGBREAK()
#endif

// ============================================================================
// Base Utilities
// ============================================================================

/// @def RS_EXPAND_MACRO(x)
/// @brief Forces expansion of a macro argument before further processing.
/// @details Required by the C++17 argument-counting trick; the MSVC
///          preprocessor would otherwise treat the comma-separated list
///          as a single token.
#define RS_EXPAND_MACRO(x) x

/// @def RS_STRINGIFY_MACRO(x)
/// @brief Converts a macro argument to a string literal.
/// @details Used to embed the textual representation of an expression
///          in assertion failure messages (e.g. `"ptr != nullptr"`).
#define RS_STRINGIFY_MACRO(x) #x

/// @brief Utility macro for defining bit flags.
#define BIT(x) (1 << x)

namespace RS
{

    // Convenience alias for unique pointers (stack-like ownership)
    template <typename T>
    using Scope = ::std::unique_ptr<T>;

    // Convenience alias for shared pointers (reference-counted ownership)
    template <typename T>
    using Shared = ::std::shared_ptr<T>;

    // Factory function: Creates a unique_ptr with perfect forwarding
    // Usage: auto myObj = CreateScope<MyClass>(arg1, arg2);
    template <typename T, typename... Args>
    [[nodiscard]] constexpr Scope<T> CreateScope(Args &&...args)
    {
        return ::std::make_unique<T>(::std::forward<Args>(args)...);
    }

    // Factory function: Creates a shared_ptr with perfect forwarding
    // Usage: auto myObj = CreateShared<MyClass>(arg1, arg2);
    template <typename T, typename... Args>
    [[nodiscard]] constexpr Shared<T> CreateShared(Args &&...args)
    {
        return ::std::make_shared<T>(::std::forward<Args>(args)...);
    }

} // namespace RS

#include "Raysim/Core/Log.hpp"
#include "Raysim/Core/Assert.hpp"
