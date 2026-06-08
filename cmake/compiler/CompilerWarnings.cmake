# ===========================================================================
# Defines the `rs_warnings` INTERFACE target that centralises the project's
# warning policy. Link any first-party target against it to inherit all flags.
#
# Usage:
#   target_link_libraries(my_target PRIVATE rs_warnings)
#
# Reference:
#   https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
# ===========================================================================

include_guard()

add_library(rs_warnings INTERFACE)
add_library(rs::warnings ALIAS rs_warnings)

# ---------------------------------------------------------------------------
# Flag sets
# ---------------------------------------------------------------------------

set(_RS_COMMON_WARNINGS         # GCC + Clang
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow                    # Local variable shadows outer scope
  -Wnon-virtual-dtor          # Virtual class without virtual destructor
  -Wold-style-cast            # C-style casts (prefer static_cast / reinterpret_cast)
  -Wcast-align                # Misaligned pointer casts (potential perf issue)
  -Wunused                    # Unused variables, parameters, functions
  -Woverloaded-virtual        # Hidden virtual (overloaded instead of overridden)
  -Wconversion                # Implicit narrowing conversions
  -Wsign-conversion           # Signed ↔ unsigned implicit conversions
  -Wnull-dereference          # Statically-detectable null dereferences
  -Wdouble-promotion          # Implicit float -> double promotion
  -Wformat=2                  # Strict printf/scanf format-string checking
)

set(_RS_CLANG_WARNINGS
  -Wdocumentation             # Malformed Doxygen / documentation comments
  -Wcomma                     # Suspicious comma-operator usage
)

set(_RS_GCC_WARNINGS
  -Wmisleading-indentation    # Indentation does not match block structure
  -Wduplicated-cond           # Identical conditions in if / else-if chain
  -Wduplicated-branches       # Both branches of if/else are identical
  -Wlogical-op                # Suspicious use of logical operators
  -Wuseless-cast              # Cast to the same type
)

set(_RS_MSVC_WARNINGS
  /W4                         # High warning level (recommended baseline)
  /permissive-                # Enforce standards conformance (also set globally)
  # --- Type safety / conversions ---
  /w14242                     # Possible data loss during conversion
  /w14254                     # Bitfield conversion may lose data
  /w14287                     # Unsigned / negative constant mismatch
  /w14826                     # Sign-extension may cause unexpected behaviour
  # --- Virtual / OOP correctness ---
  /w14263                     # Member function does not override base virtual
  /w14265                     # Virtual functions but no virtual destructor
  # --- Logic / correctness ---
  /w14296                     # Expression always evaluates to a constant
  /w14311                     # Pointer truncation to smaller type
  /w14640                     # Thread-unsafe static member initialisation
  # --- Comma / expression side-effects ---
  /w14545                     # Function call missing argument list before comma
  /w14546                     # Function call missing argument list
  /w14547                     # Operator before comma has no effect
  /w14549                     # Suspicious comma-operator usage
  /w14555                     # Expression has no effect; expected side-effect
  # --- Miscellaneous ---
  /w14619                     # Invalid pragma warning number
  /w14905                     # Wide string literal cast to LPSTR
  /w14906                     # String literal cast to LPWSTR
  /w14928                     # Illegal copy-initialisation (multiple conversions)
)

# ---------------------------------------------------------------------------
# Apply flags via generator expressions
# ---------------------------------------------------------------------------

target_compile_options(rs_warnings INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:${_RS_COMMON_WARNINGS}>
  $<$<CXX_COMPILER_ID:Clang,AppleClang>:${_RS_CLANG_WARNINGS}>
  $<$<CXX_COMPILER_ID:GNU>:${_RS_GCC_WARNINGS}>
  $<$<CXX_COMPILER_ID:MSVC>:${_RS_MSVC_WARNINGS}>
)

# ---------------------------------------------------------------------------
# Warnings as errors  (opt-in via RS_WARNINGS_AS_ERRORS)
# ---------------------------------------------------------------------------

if(RS_WARNINGS_AS_ERRORS)
  target_compile_options(rs_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
  )
  message(STATUS "[rs] Warnings treated as errors: ON")
endif()

message(STATUS "[rs] Warnings configured")
