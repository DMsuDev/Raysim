# ================================================
# Warnings configuration
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
# ================================================

function(set_project_warnings project_name)
  if(NOT TARGET ${project_name})
    message(FATAL_ERROR "Target '${project_name}' does not exist")
  endif()

  # Common warnings for all compilers
  set(COMMON_WARNINGS
    -Wall                # Enable most basic warnings
    -Wextra              # Extra warnings not included in -Wall
    -Wshadow             # Warn if variable shadows another variable
    -Wnon-virtual-dtor   # Warn if class with virtual functions lacks virtual destructor
    -Wold-style-cast     # Warn on C-style casts (prefer static_cast, etc.)
    -Wcast-align         # Warn for potential performance issues due to misaligned casts
    -Wunused             # Warn on unused variables, parameters, functions
    -Woverloaded-virtual # Warn when a virtual function is hidden instead of overridden
    -Wpedantic           # Enforce strict ISO C++ compliance
    -Wconversion         # Warn on implicit type conversions that may lose data
    -Wsign-conversion    # Warn on sign changes (unsigned ↔ signed)
    -Wnull-dereference   # Warn if null pointer dereference is detected
    -Wdouble-promotion   # Warn when float is implicitly promoted to double
    -Wformat=2           # Strong format string checking (printf-like)
  )

  set(CLANG_WARNINGS
    ${COMMON_WARNINGS}
    -Wdocumentation      # Warn about invalid/malformed Doxygen comments
    -Wcomma              # Warn about suspicious comma operator usage
  )

  set(GCC_WARNINGS
    ${COMMON_WARNINGS}

    -Wmisleading-indentation # Warn if indentation does not match actual code blocks
    -Wduplicated-cond        # Warn if identical conditions appear in if/else chains
    -Wduplicated-branches    # Warn if both branches of if/else are identical
    -Wlogical-op             # Warn about suspicious logical expressions
    -Wuseless-cast           # Warn when casting to the same type
  )

  set(MSVC_WARNINGS
    /W4           # High warning level (recommended baseline)
    # Type safety / conversions
    /w14242       # Possible loss of data during conversion
    /w14254       # Bitfield conversion may lose data
    /w14287       # Unsigned/negative mismatch
    # Virtual / OOP correctness
    /w14263       # Function does not override base virtual function
    /w14265       # Class has virtual functions but no virtual destructor
    # Logic / correctness
    /w14296       # Expression always evaluates to constant
    /w14311       # Pointer truncation
    /w14826       # Sign-extension may cause unexpected behavior
    # Comma / expressions
    /w14545       # Function missing argument list before comma
    /w14546       # Function call missing argument list
    /w14547       # Operator before comma has no effect
    /w14549       # Suspicious comma operator usage
    /w14555       # Expression has no effect
    # Misc
    /w14619       # Invalid pragma warning number
    /w14640       # Thread-unsafe static initialization
    /w14905       # Wide string cast to narrow string
    /w14906       # Narrow string cast to wide string
    /w14928       # Illegal copy initialization (multiple conversions)
  )

  if(RS_WARNINGS_AS_ERRORS)
    list(APPEND CLANG_WARNINGS -Werror)
    list(APPEND GCC_WARNINGS -Werror)
    list(APPEND MSVC_WARNINGS /WX)
  endif()

  if(MSVC)
    set(PROJECT_WARNINGS ${MSVC_WARNINGS})

  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})

  else()
    message(AUTHOR_WARNING
      "No warning configuration for compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()

  target_compile_options(${project_name} PRIVATE ${PROJECT_WARNINGS})

endfunction()
