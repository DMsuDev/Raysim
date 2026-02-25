#pragma once

// Assert.hpp - Comprehensive Asserts System for RaySim
//
// FEATURES:
// - Optional support for std::source_location (C++20) with C++17 fallback
// - Logging via LOG_ERROR macros
// - Multiple assertion variants with automatic message generation
// - Helpful macros: NOT_NULL, IN_RANGE, TRUE, FALSE, EQUAL, NOT_EQUAL
// - Smart macro selector for variadic argument handling
// - Optimized debug breaks and zero-cost release builds
// - Configurable via RS_ENABLE_ASSERTS (ON in Debug by default)
//
// USAGE EXAMPLES:
//   RS_ASSERT(x > 0);
//   RS_ASSERT(ptr != nullptr, "Custom message");
//   RS_ASSERT(value < max, "Value {} exceeds max {}", value, max);
//   RS_ASSERT_NOT_NULL(ptr);
//   RS_ASSERT_IN_RANGE(value, 0, 100);
//   RS_ASSERT_EQUAL(actual, expected);

#include "Raysim/Utils/Log.hpp"

// ============================================================================
// Base Utilities
// ============================================================================

// Macro expansion helper - forces macro argument expansion
#define RS_EXPAND_MACRO(x) x

// Stringify macro - converts macro argument to string literal
#define RS_STRINGIFY_MACRO(x) #x

// Debug break - platform-specific debugger break
#ifdef _WIN32
    #define RS_DEBUGBREAK() __debugbreak()
#elif __GNUC__
    #define RS_DEBUGBREAK() __builtin_trap()
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
        const char* file_name() const { return file; }
        unsigned line() const { return line_; }
        const char* function_name() const { return func; }

        const char* file;
        unsigned line_;
        const char* func;
    };
    #define RS_CURRENT_LOC() rs_assert_loc_t{__FILE__, __LINE__, __func__}
    #define RS_ASSERT_HAS_SOURCE_LOCATION 0
#endif

// ============================================================================
// Internal implementation macros - DO NOT USE DIRECTLY
// ============================================================================
#ifdef RS_ENABLE_ASSERTS

    // Base implementation: logs and breaks on assertion failure
    // check: boolean condition
    // loc: source location (file, line, function)
    // msg: format string for custom message
    #define RS_INTERNAL_ASSERT_IMPL(check, loc, msg, ...) \
        do { \
            if (!(check)) { \
                LOG_ERROR("Assertion failed at {}:{} ({})", \
                    (loc).file_name(), (loc).line(), (loc).function_name()); \
                LOG_ERROR(msg, ##__VA_ARGS__); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    // Variant 1: No custom message - auto-generates from condition
    #define RS_INTERNAL_ASSERT_NO_MSG(check) \
        RS_INTERNAL_ASSERT_IMPL(check, RS_CURRENT_LOC(), \
            "Condition: '{}'", RS_STRINGIFY_MACRO(check))

    // Variant 2 & 3: With message/formatting
    #define RS_INTERNAL_ASSERT_WITH_MSG(check, msg, ...) \
        RS_INTERNAL_ASSERT_IMPL(check, RS_CURRENT_LOC(), msg, ##__VA_ARGS__)

    #define RS_INTERNAL_ASSERT_WITH_FMT(check, fmt, ...) \
        RS_INTERNAL_ASSERT_IMPL(check, RS_CURRENT_LOC(), fmt, ##__VA_ARGS__)

    // Smart macro selector - detects number of arguments and chooses appropriate variant
    #define RS_INTERNAL_ASSERT_GET_MACRO(_1, _2, _3, NAME, ...) NAME

    #define RS_INTERNAL_ASSERT_CHOOSER(...) \
        RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, \
            RS_INTERNAL_ASSERT_WITH_FMT, \
            RS_INTERNAL_ASSERT_WITH_MSG, \
            RS_INTERNAL_ASSERT_NO_MSG ))

    // Main public assertion macro
    #define RS_ASSERT(...) \
        RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__) )

    // Specialized assertion helpers - common patterns made concise

    // Pointer validation
    #define RS_ASSERT_NOT_NULL(ptr) \
        RS_ASSERT((ptr) != nullptr, "Pointer must not be null: {}", RS_STRINGIFY_MACRO(ptr))

    // Range validation
    #define RS_ASSERT_IN_RANGE(val, min_val, max_val) \
        RS_ASSERT((val) >= (min_val) && (val) <= (max_val), \
            "Value {} is out of range [{}, {}]", (val), (min_val), (max_val))

    // Boolean assertions
    #define RS_ASSERT_TRUE(condition) \
        RS_ASSERT((condition), "Expected true, but condition is false: {}", RS_STRINGIFY_MACRO(condition))

    #define RS_ASSERT_FALSE(condition) \
        RS_ASSERT(!(condition), "Expected false, but condition is true: {}", RS_STRINGIFY_MACRO(condition))

    // Equality assertions
    #define RS_ASSERT_EQUAL(actual, expected) \
        RS_ASSERT((actual) == (expected), \
            "Values not equal - actual: {}, expected: {}", (actual), (expected))

    #define RS_ASSERT_NOT_EQUAL(actual, unexpected) \
        RS_ASSERT((actual) != (unexpected), \
            "Values should not be equal: {}", (actual))

#else

    // Release build: all assertions become no-ops with zero runtime cost
    #define RS_ASSERT(...)              ((void)0)
    #define RS_ASSERT_NOT_NULL(...)     ((void)0)
    #define RS_ASSERT_IN_RANGE(...)     ((void)0)
    #define RS_ASSERT_TRUE(...)         ((void)0)
    #define RS_ASSERT_FALSE(...)        ((void)0)
    #define RS_ASSERT_EQUAL(...)        ((void)0)
    #define RS_ASSERT_NOT_EQUAL(...)    ((void)0)

#endif

/*
COMPREHENSIVE USAGE EXAMPLES:

Basic assertions:
    RS_ASSERT(x > 0);                               // Auto-message from condition
    RS_ASSERT(ptr != nullptr, "Critical pointer");  // With custom message
    RS_ASSERT(value < max, "Value {} exceeds {}", value, max);  // With formatting

Pointer validation:
    RS_ASSERT_NOT_NULL(ptr);                        // Ensures pointer is valid

Boolean assertions:
    RS_ASSERT_TRUE(initialized);                    // Condition must be true
    RS_ASSERT_FALSE(error_flag);                    // Condition must be false

Value validation:
    RS_ASSERT_IN_RANGE(index, 0, size - 1);        // Value within bounds
    RS_ASSERT_EQUAL(actual_value, expected);        // Exact equality check
    RS_ASSERT_NOT_EQUAL(status, ERROR_CODE);       // Inequality check

PRACTICAL EXAMPLE - Vector access:
    void setElement(std::vector<int>& vec, size_t index, int value) {
        RS_ASSERT_NOT_NULL(&vec);
        RS_ASSERT_IN_RANGE(index, 0, vec.size() - 1);
        vec[index] = value;
    }

PRACTICAL EXAMPLE - State machine:
    void setState(State newState) {
        RS_ASSERT_NOT_EQUAL(newState, currentState);
        RS_ASSERT_EQUAL(validated, true);
        currentState = newState;
    }

NOTES:
- All asserts compile to zero-cost no-ops in Release builds
- Location info (file, line, function) is automatically captured
- C++20 uses std::source_location for best compatibility
- C++17 falls back to manual location macros (no runtime cost)
*/
