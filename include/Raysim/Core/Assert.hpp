#pragma once

#include "Logger.hpp"

// ============================================================================
// Base Utilities
// ============================================================================

// Macro expansion helper - forces macro argument expansion
// e.g. RS_EXPAND_MACRO(MACRO(ARG)) -> expands MACRO(ARG) before stringification
#define RS_EXPAND_MACRO(x) x

// Stringify macro - converts macro argument to string literal
// e.g. RS_STRINGIFY_MACRO(a == b) -> "a == b"
#define RS_STRINGIFY_MACRO(x) #x

// Debug break - platform-specific debugger break
#ifdef RS_DEBUG
	#ifdef _WIN32
		#define RS_DEBUGBREAK() __debugbreak()
	#elif defined(__linux__)
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
// Source Location Support
// ============================================================================

// Support for source_location (C++20) or lightweight fallback for older standards
#if __cplusplus >= 202002L
    #include <source_location>
    using rs_assert_loc_t = std::source_location;
    #define RS_CURRENT_LOC() rs_assert_loc_t::current()
    #define RS_ASSERT_HAS_SOURCE_LOCATION 1
#else
    // Minimal fallback structure - zero overhead in C++17
    struct rs_assert_loc_t {
        constexpr const char* file_name() const { return file; }
        constexpr unsigned line() const { return line_; }
        constexpr const char* function_name() const { return func; }

        const char* file;
        unsigned line_;
        const char* func;
    };
    #define RS_CURRENT_LOC() rs_assert_loc_t{__FILE__, __LINE__, __func__}
    #define RS_ASSERT_HAS_SOURCE_LOCATION 0
#endif

// ============================================================================
// ASSERTION IMPLEMENTATION
// ============================================================================

// Internal implementation macros - DO NOT USE DIRECTLY
// These are the core mechanisms used by public assertion macros
#ifdef RS_ENABLE_ASSERTS

// ============================================================================
// C++20 Path: Uses __VA_OPT__ for cleaner variadic handling
// ============================================================================
#if __cplusplus >= 202002L

    // Base implementation: logs and breaks on assertion failure
    #define RS_INTERNAL_ASSERT_IMPL(check, loc, msg, ...) \
        do { \
            if (!(check)) { \
                LOG_ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                LOG_ERROR(msg __VA_OPT__(,) __VA_ARGS__); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    // Public assertion macro - accepts (check) or (check, msg, ...)
    #define RS_ASSERT(check, ...) \
        RS_INTERNAL_ASSERT_IMPL(check, RS_CURRENT_LOC(), \
            __VA_ARGS__ __VA_OPT__(,) "No additional message")

// ============================================================================
// C++17 Path: Uses variant selection for compatibility
// ============================================================================
#else

    // Base implementation without variadic args - used when there's no format args
    #define RS_INTERNAL_ASSERT_IMPL_NO_ARGS(check, loc, msg) \
        do { \
            if (!(check)) { \
                LOG_ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                LOG_ERROR("{}", msg); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    // Base implementation with variadic args - used when format args are present
    #define RS_INTERNAL_ASSERT_IMPL_WITH_ARGS(check, loc, msg, ...) \
        do { \
            if (!(check)) { \
                LOG_ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                LOG_ERROR(msg, __VA_ARGS__); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    // Variant 1: No custom message
    #define RS_INTERNAL_ASSERT_NO_MSG(check) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(check, RS_CURRENT_LOC(), "No additional message")

    // Variant 2: Custom message without format args
    #define RS_INTERNAL_ASSERT_WITH_MSG_ONLY(check, msg) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(check, RS_CURRENT_LOC(), msg)

    // Variant 3: Custom message with format args
    #define RS_INTERNAL_ASSERT_WITH_MSG_ARGS(check, msg, ...) \
        RS_INTERNAL_ASSERT_IMPL_WITH_ARGS(check, RS_CURRENT_LOC(), msg, __VA_ARGS__)

    // Smart macro selector - detects number of arguments and chooses appropriate variant
    #define RS_INTERNAL_ASSERT_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

    #define RS_INTERNAL_ASSERT_CHOOSER(...) \
        RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_ASSERT_WITH_MSG_ONLY, \
            RS_INTERNAL_ASSERT_NO_MSG ))

    // Main public assertion macro - works with all variants
    #define RS_ASSERT(...) \
        RS_EXPAND_MACRO(RS_INTERNAL_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#endif

// ============================================================================
// Specialized assertion helpers - common patterns made concise
// ============================================================================

// Pointer validation
#define RS_ASSERT_NOT_NULL(ptr) \
    RS_ASSERT((ptr) != nullptr, "Pointer must not be null: {}", RS_STRINGIFY_MACRO(ptr))


// Range validation
#define RS_ASSERT_IN_RANGE(val, min_val, max_val) \
    RS_ASSERT(((val) >= (min_val)) && ((val) <= (max_val)), \
        "Value '{}' is out of range [{} - {}] (check: {} >= {} && {} <= {})", \
        val, min_val, max_val, \
        RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(min_val), RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(max_val));

// Boolean assertions
#define RS_ASSERT_TRUE(condition) \
    RS_ASSERT((condition), "Expected true but condition is false: {}", RS_STRINGIFY_MACRO(condition))


#define RS_ASSERT_FALSE(condition) \
    RS_ASSERT(!(condition), "Expected false but condition is true: {}", RS_STRINGIFY_MACRO(condition))


// Equality assertions
#define RS_ASSERT_EQUAL(actual, expected) \
    RS_ASSERT((actual) == (expected), \
        "Values not equal - actual: {}, expected: {}", (actual), (expected))


#define RS_ASSERT_NOT_EQUAL(actual, unexpected) \
    RS_ASSERT((actual) != (unexpected), \
        "Values should not be equal: {}", (actual))

// Exception validation
#define RS_ASSERT_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            auto loc = RS_CURRENT_LOC(); \
            LOG_ERROR("Assertion threw exception at {}:{} ({}): {}", \
                loc.file_name(), loc.line(), loc.function_name(), e.what()); \
            throw; \
        } catch (...) { \
            auto loc = RS_CURRENT_LOC(); \
            LOG_ERROR("Assertion threw unknown exception at {}:{} ({})", \
                loc.file_name(), loc.line(), loc.function_name()); \
            throw; \
        } \
    } while (0)
#else

// Release build: all assertions become no-ops with zero runtime cost
#define RS_ASSERT(...)              ((void)0)
#define RS_ASSERT_NOT_NULL(...)     ((void)0)
#define RS_ASSERT_IN_RANGE(...)     ((void)0)
#define RS_ASSERT_TRUE(...)         ((void)0)
#define RS_ASSERT_FALSE(...)        ((void)0)
#define RS_ASSERT_EQUAL(...)        ((void)0)
#define RS_ASSERT_NOT_EQUAL(...)    ((void)0)
#define RS_ASSERT_THROW(...)        ((void)0)

#endif
