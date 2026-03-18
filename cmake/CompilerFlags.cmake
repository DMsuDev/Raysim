# ============================================================================
# Compiler-specific warnings and flags
# ============================================================================

set(WARNING_FLAGS "")

# -------------------------------
# GCC / Clang
# -------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    list(APPEND WARNING_FLAGS
        -Wall                     # All common warnings
        -Wextra                   # Extra warnings beyond -Wall
        -Wpedantic                # Warnings about non-standard C++
        -Wshadow                  # Variable shadowing
        -Wconversion              # Implicit type conversions
        -Wnon-virtual-dtor        # Missing virtual destructors
        -Wold-style-cast          # C-style casts
        -Wcast-align              # Cast alignment issues
        -Woverloaded-virtual      # Overloaded virtual functions
        -Wdouble-promotion        # Implicit double promotion
        -Wformat=2                # Format string issues
        -Wmisleading-indentation  # Misleading indentation
        -Wnull-dereference        # Null pointer dereference
        -Wunused-parameter        # Unused function parameters
    )

    # Clang-specific warnings
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND WARNING_FLAGS
            -Wdocumentation
            -Wcomma
            -Wheader-hygiene
        )
    endif()

# -------------------------------
# MSVC
# -------------------------------
elseif(MSVC)
    list(APPEND WARNING_FLAGS
        /W4                 # High level of warnings (stricter than /W3)
        /EHsc               # Exception handling: synchronous C++ exceptions
        /permissive-        # Strict standard compliance
        /Zc:__cplusplus     # Correctly report __cplusplus
        /Zc:preprocessor    # Enable conformant preprocessor (__VA_OPT__ support)
        /w14062             # Unhandled enum in switch
        /w14242             # Possible data loss in conversion
        /w14254             # Different size conversion
        /w14263             # Function does not use parameter
        /w14265             # Class has unreferenced members
        /w14287             # Operator not const
        /we4289             # Non-standard variable declared in for
        /w14545             # Expression with comma
        /w14546             # Function to pointer conversion
        /w14547             # Operator before comma (probable error)
        /w14549             # Operator before comma (probable error)
        /w14555             # Expression has no effect
        /w14619             # Unknown pragma
        /w14905             # Wide conversion between signed/unsigned types
        /w14906             # Truncated conversion from integer to pointer
        /w14928             # Uninitialized variable
    )
endif()

# -------------------------------
# Optionally treat warnings as errors
# -------------------------------
if(WARNINGS_AS_ERRORS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND WARNING_FLAGS -Werror)
    elseif(MSVC)
        list(APPEND WARNING_FLAGS /WX)
    endif()
endif()

# -------------------------------
# Apply warnings globally
# -------------------------------
if(WARNING_FLAGS)
    add_compile_options(${WARNING_FLAGS})
endif()

# -------------------------------
# Informative message
# -------------------------------
message(STATUS "Enabled warning flags: ${WARNING_FLAGS}")
