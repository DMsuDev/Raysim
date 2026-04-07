/**
 * @file Assert.hpp
 * @brief Assertion macros with integrated logging and source location support.
 *
 * This header defines a set of assertion macros that provide detailed logging
 * information on assertion failures, including the expression that failed and
 * the source location (file, line, function). The macros are split into two
 * families:
 *
 * - `RS_CORE_ASSERT*`: Intended for engine/internal use, logs through the core logger.
 *
 * - `RS_ASSERT*`: Intended for client/application use, logs through the client logger
 */
#pragma once

#include "Log.hpp"

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

// Define platform-specific debug break
// In debug builds, this will trigger a breakpoint in the debugger
// when an assertion fails.
#ifdef RS_BUILD_DEBUG
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
//
// Two families of assertion macros, mirroring the Core/Client logger split:
//   RS_CORE_ASSERT*  – engine internals, logs through RS_CORE_ERROR
//   RS_ASSERT*       – client/application, logs through RS_ERROR
//
// The `type` token is pasted to form the logger macro:
//   type = _CORE_  ->  RS_CORE_ERROR(...)
//   type = _       ->  RS_ERROR(...)

/// @cond INTERNAL
// Internal implementation macros - DO NOT USE DIRECTLY
// These are the core mechanisms used by public assertion macros
#ifdef RS_ENABLE_ASSERTS

// ============================================================================
// C++20 Path: Uses __VA_OPT__ for cleaner variadic handling
// ============================================================================
#if __cplusplus >= 202002L

    #define RS_INTERNAL_ASSERT_IMPL(type, check, loc, msg, ...) \
        do { \
            if (!(check)) { \
                RS##type##ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                RS##type##ERROR(msg __VA_OPT__(,) __VA_ARGS__); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    /// @endcond

    /// @def RS_CORE_ASSERT(check, ...)
    /// @brief Assert a condition in **engine** code. Logs via core logger on failure.
    /// @param check  Boolean expression to evaluate.
    /// @param ...    Optional fmt-style message and format arguments.
    ///
    /// On failure: logs the stringified expression with source location,
    /// logs the user message (if any), then triggers a debugger break.
    /// Compiles to nothing in Release builds.
    #define RS_CORE_ASSERT(check, ...) \
        RS_INTERNAL_ASSERT_IMPL(_CORE_, check, RS_CURRENT_LOC(), \
            __VA_ARGS__ __VA_OPT__(,) "No additional message")

    /// @def RS_ASSERT(check, ...)
    /// @brief Assert a condition in **client/application** code. Logs via client logger on failure.
    /// @param check  Boolean expression to evaluate.
    /// @param ...    Optional fmt-style message and format arguments.
    ///
    /// Identical behavior to `RS_CORE_ASSERT` but routes output through
    /// the client logger ("APP") instead of the core logger ("RAYSIM").
    /// Compiles to nothing in Release builds.
    #define RS_ASSERT(check, ...) \
        RS_INTERNAL_ASSERT_IMPL(_, check, RS_CURRENT_LOC(), \
            __VA_ARGS__ __VA_OPT__(,) "No additional message")

// ============================================================================
// C++17 Path: Uses variant selection for compatibility
// ============================================================================
#else

    /// @cond INTERNAL
    #define RS_INTERNAL_ASSERT_IMPL_NO_ARGS(type, check, loc, msg) \
        do { \
            if (!(check)) { \
                RS##type##ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                RS##type##ERROR("{}", msg); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    #define RS_INTERNAL_ASSERT_IMPL_WITH_ARGS(type, check, loc, msg, ...) \
        do { \
            if (!(check)) { \
                RS##type##ERROR("Assertion failed: '{}' at {}:{} ({})", \
                    RS_STRINGIFY_MACRO(check), \
                    (loc).file_name(), \
                    (loc).line(), \
                    (loc).function_name()); \
                RS##type##ERROR(msg, __VA_ARGS__); \
                RS_DEBUGBREAK(); \
            } \
        } while (0)

    // --- Core assertion variants (engine internals) --------------------------

    #define RS_INTERNAL_CORE_ASSERT_NO_MSG(check) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(_CORE_, check, RS_CURRENT_LOC(), "No additional message")

    #define RS_INTERNAL_CORE_ASSERT_WITH_MSG_ONLY(check, msg) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(_CORE_, check, RS_CURRENT_LOC(), msg)

    #define RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS(check, msg, ...) \
        RS_INTERNAL_ASSERT_IMPL_WITH_ARGS(_CORE_, check, RS_CURRENT_LOC(), msg, __VA_ARGS__)

    // --- Client assertion variants (application code) ------------------------

    #define RS_INTERNAL_CLIENT_ASSERT_NO_MSG(check) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(_, check, RS_CURRENT_LOC(), "No additional message")

    #define RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ONLY(check, msg) \
        RS_INTERNAL_ASSERT_IMPL_NO_ARGS(_, check, RS_CURRENT_LOC(), msg)

    #define RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS(check, msg, ...) \
        RS_INTERNAL_ASSERT_IMPL_WITH_ARGS(_, check, RS_CURRENT_LOC(), msg, __VA_ARGS__)

    #define RS_INTERNAL_ASSERT_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

    #define RS_INTERNAL_CORE_ASSERT_CHOOSER(...) \
        RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CORE_ASSERT_WITH_MSG_ONLY, \
            RS_INTERNAL_CORE_ASSERT_NO_MSG ))

    #define RS_INTERNAL_CLIENT_ASSERT_CHOOSER(...) \
        RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ARGS, \
            RS_INTERNAL_CLIENT_ASSERT_WITH_MSG_ONLY, \
            RS_INTERNAL_CLIENT_ASSERT_NO_MSG ))
    /// @endcond

    /// @def RS_CORE_ASSERT(...)
    /// @brief Assert a condition in **engine** code (C++17 path).
    /// @copydetails RS_CORE_ASSERT
    #define RS_CORE_ASSERT(...) \
        RS_EXPAND_MACRO(RS_INTERNAL_CORE_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

    /// @def RS_ASSERT(...)
    /// @brief Assert a condition in **client/application** code (C++17 path).
    /// @copydetails RS_ASSERT
    #define RS_ASSERT(...) \
        RS_EXPAND_MACRO(RS_INTERNAL_CLIENT_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#endif

// ============================================================================
// Specialized Assertion Helpers
// ============================================================================
/// @name Pointer Validation
/// @{

/// @def RS_CORE_ASSERT_NOT_NULL(ptr)
/// @brief Asserts that @p ptr is not `nullptr` (core logger).
/// @param ptr  Pointer expression to validate.
#define RS_CORE_ASSERT_NOT_NULL(ptr) \
    RS_CORE_ASSERT((ptr) != nullptr, "Pointer must not be null: {}", RS_STRINGIFY_MACRO(ptr))

/// @def RS_ASSERT_NOT_NULL(ptr)
/// @brief Asserts that @p ptr is not `nullptr` (client logger).
/// @param ptr  Pointer expression to validate.
#define RS_ASSERT_NOT_NULL(ptr) \
    RS_ASSERT((ptr) != nullptr, "Pointer must not be null: {}", RS_STRINGIFY_MACRO(ptr))

/// @}
/// @name Range Validation
/// @{

/// @def RS_CORE_ASSERT_IN_RANGE(val, min_val, max_val)
/// @brief Asserts that @p val is within [`min_val`, `max_val`] inclusive (core logger).
/// @param val      Value to check.
/// @param min_val  Lower bound (inclusive).
/// @param max_val  Upper bound (inclusive).
#define RS_CORE_ASSERT_IN_RANGE(val, min_val, max_val) \
    RS_CORE_ASSERT(((val) >= (min_val)) && ((val) <= (max_val)), \
        "Value '{}' is out of range [{} - {}] (check: {} >= {} && {} <= {})", \
        val, min_val, max_val, \
        RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(min_val), RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(max_val));

/// @def RS_ASSERT_IN_RANGE(val, min_val, max_val)
/// @brief Asserts that @p val is within [`min_val`, `max_val`] inclusive (client logger).
/// @param val      Value to check.
/// @param min_val  Lower bound (inclusive).
/// @param max_val  Upper bound (inclusive).
#define RS_ASSERT_IN_RANGE(val, min_val, max_val) \
    RS_ASSERT(((val) >= (min_val)) && ((val) <= (max_val)), \
        "Value '{}' is out of range [{} - {}] (check: {} >= {} && {} <= {})", \
        val, min_val, max_val, \
        RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(min_val), RS_STRINGIFY_MACRO(val), RS_STRINGIFY_MACRO(max_val));

/// @}
/// @name Boolean Assertions
/// @{

/// @def RS_CORE_ASSERT_TRUE(condition)
/// @brief Asserts that @p condition evaluates to `true` (core logger).
/// @param condition  Boolean expression expected to be true.
#define RS_CORE_ASSERT_TRUE(condition) \
    RS_CORE_ASSERT((condition), "Expected true but condition is false: {}", RS_STRINGIFY_MACRO(condition))

/// @def RS_ASSERT_TRUE(condition)
/// @brief Asserts that @p condition evaluates to `true` (client logger).
/// @param condition  Boolean expression expected to be true.
#define RS_ASSERT_TRUE(condition) \
    RS_ASSERT((condition), "Expected true but condition is false: {}", RS_STRINGIFY_MACRO(condition))

/// @def RS_CORE_ASSERT_FALSE(condition)
/// @brief Asserts that @p condition evaluates to `false` (core logger).
/// @param condition  Boolean expression expected to be false.
#define RS_CORE_ASSERT_FALSE(condition) \
    RS_CORE_ASSERT(!(condition), "Expected false but condition is true: {}", RS_STRINGIFY_MACRO(condition))

/// @def RS_ASSERT_FALSE(condition)
/// @brief Asserts that @p condition evaluates to `false` (client logger).
/// @param condition  Boolean expression expected to be false.
#define RS_ASSERT_FALSE(condition) \
    RS_ASSERT(!(condition), "Expected false but condition is true: {}", RS_STRINGIFY_MACRO(condition))

/// @}
/// @name Equality Assertions
/// @{

/// @def RS_CORE_ASSERT_EQUAL(actual, expected)
/// @brief Asserts that @p actual equals @p expected (core logger).
/// @param actual    Value produced by the code under test.
/// @param expected  Value that @p actual must match.
#define RS_CORE_ASSERT_EQUAL(actual, expected) \
    RS_CORE_ASSERT((actual) == (expected), \
        "Values not equal - actual: {}, expected: {}", (actual), (expected))

/// @def RS_ASSERT_EQUAL(actual, expected)
/// @brief Asserts that @p actual equals @p expected (client logger).
/// @param actual    Value produced by the code under test.
/// @param expected  Value that @p actual must match.
#define RS_ASSERT_EQUAL(actual, expected) \
    RS_ASSERT((actual) == (expected), \
        "Values not equal - actual: {}, expected: {}", (actual), (expected))

/// @def RS_CORE_ASSERT_NOT_EQUAL(actual, unexpected)
/// @brief Asserts that @p actual does **not** equal @p unexpected (core logger).
/// @param actual      Value produced by the code under test.
/// @param unexpected  Value that @p actual must differ from.
#define RS_CORE_ASSERT_NOT_EQUAL(actual, unexpected) \
    RS_CORE_ASSERT((actual) != (unexpected), \
        "Values should not be equal: {}", (actual))

/// @def RS_ASSERT_NOT_EQUAL(actual, unexpected)
/// @brief Asserts that @p actual does **not** equal @p unexpected (client logger).
/// @param actual      Value produced by the code under test.
/// @param unexpected  Value that @p actual must differ from.
#define RS_ASSERT_NOT_EQUAL(actual, unexpected) \
    RS_ASSERT((actual) != (unexpected), \
        "Values should not be equal: {}", (actual))

/// @}
/// @name Exception Validation
/// @{

/// @def RS_CORE_ASSERT_THROW(expr)
/// @brief Wraps @p expr in a try/catch; logs and re-throws any exception (core logger).
/// @param expr  Expression that should not throw during normal execution.
///
/// Useful for guarding calls that are expected to succeed but whose failure
/// needs to be captured in the engine log before propagating.
#define RS_CORE_ASSERT_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            auto loc = RS_CURRENT_LOC(); \
            RS_CORE_ERROR("Assertion threw exception at {}:{} ({}): {}", \
                loc.file_name(), loc.line(), loc.function_name(), e.what()); \
            throw; \
        } catch (...) { \
            auto loc = RS_CURRENT_LOC(); \
            RS_CORE_ERROR("Assertion threw unknown exception at {}:{} ({})", \
                loc.file_name(), loc.line(), loc.function_name()); \
            throw; \
        } \
    } while (0)

/// @def RS_ASSERT_THROW(expr)
/// @brief Wraps @p expr in a try/catch; logs and re-throws any exception (client logger).
/// @param expr  Expression that should not throw during normal execution.
#define RS_ASSERT_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            auto loc = RS_CURRENT_LOC(); \
            RS_ERROR("Assertion threw exception at {}:{} ({}): {}", \
                loc.file_name(), loc.line(), loc.function_name(), e.what()); \
            throw; \
        } catch (...) { \
            auto loc = RS_CURRENT_LOC(); \
            RS_ERROR("Assertion threw unknown exception at {}:{} ({})", \
                loc.file_name(), loc.line(), loc.function_name()); \
            throw; \
        } \
    } while (0)

/// @}

#else

// ============================================================================
// Release Build: No-Op Stubs
// ============================================================================
// In Release builds (`RS_ENABLE_ASSERTS` not defined), every assertion macro
// expands to `((void)0)`: zero runtime cost, zero binary bloat.

#define RS_CORE_ASSERT(...)              ((void)0)
#define RS_CORE_ASSERT_NOT_NULL(...)     ((void)0)
#define RS_CORE_ASSERT_IN_RANGE(...)     ((void)0)
#define RS_CORE_ASSERT_TRUE(...)         ((void)0)
#define RS_CORE_ASSERT_FALSE(...)        ((void)0)
#define RS_CORE_ASSERT_EQUAL(...)        ((void)0)
#define RS_CORE_ASSERT_NOT_EQUAL(...)    ((void)0)
#define RS_CORE_ASSERT_THROW(...)        ((void)0)

#define RS_ASSERT(...)                   ((void)0)
#define RS_ASSERT_NOT_NULL(...)          ((void)0)
#define RS_ASSERT_IN_RANGE(...)          ((void)0)
#define RS_ASSERT_TRUE(...)              ((void)0)
#define RS_ASSERT_FALSE(...)             ((void)0)
#define RS_ASSERT_EQUAL(...)             ((void)0)
#define RS_ASSERT_NOT_EQUAL(...)         ((void)0)
#define RS_ASSERT_THROW(...)             ((void)0)

/// @endcond

#endif
