# ================================================
# C++ Sanitizers configuration
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
# ================================================

include_guard()

function(enable_sanitizers target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_SANITIZERS)
    message(STATUS "Sanitizers: Disabled")
    return()
  endif()

  message(STATUS "Sanitizers: Enabled")

  # Warn if sanitizers are enabled in a non-Debug build
  if(NOT CMAKE_CONFIGURATION_TYPES AND
     NOT CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    message(WARNING "Sanitizers are usually used with Debug builds")
  endif()

  # Compiler-specific flags
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    # Sanitizers are not reliably supported under MinGW on Windows
    if(MINGW)
      message(WARNING "Sanitizers are not supported with MinGW -> skipping")
      return()
    endif()

    set(SANITIZERS)
    set(SANITIZER_COMPILE_FLAGS)
    set(SANITIZER_LINK_FLAGS)

    # Compatibility checks
    if(RS_ENABLE_TSAN AND (RS_ENABLE_ASAN OR RS_ENABLE_MSAN))
      message(FATAL_ERROR "TSan cannot be combined with ASan or MSan")
    endif()

    if(RS_ENABLE_MSAN AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      message(FATAL_ERROR "MemorySanitizer requires Clang")
    endif()

    # Determine which sanitizers to enable
    if(RS_ENABLE_ASAN)
      list(APPEND SANITIZERS address)
    endif()

    if(RS_ENABLE_UBSAN)
      list(APPEND SANITIZERS undefined)
    endif()

    if(RS_ENABLE_TSAN)
      list(APPEND SANITIZERS thread)
    endif()

    if(RS_ENABLE_MSAN)
      list(APPEND SANITIZERS memory)
    endif()

    # LeakSanitizer (only if ASan not enabled)
    if(RS_ENABLE_LSAN AND NOT RS_ENABLE_ASAN)
      list(APPEND SANITIZERS leak)
    endif()

    if(SANITIZERS)
      list(JOIN SANITIZERS "," SANITIZER_LIST)

      message(STATUS "Sanitizers enabled: ${SANITIZER_LIST}")

      list(APPEND SANITIZER_COMPILE_FLAGS
        -fsanitize=${SANITIZER_LIST}
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
        -fno-common
      )

      list(APPEND SANITIZER_LINK_FLAGS
        -fsanitize=${SANITIZER_LIST}
      )

      target_compile_options(${target_name} INTERFACE ${SANITIZER_COMPILE_FLAGS})
      target_link_options(${target_name} INTERFACE ${SANITIZER_LINK_FLAGS})
    endif()

  elseif(MSVC)

    # MSVC only supports AddressSanitizer
    if(RS_ENABLE_ASAN)
      message(STATUS "MSVC AddressSanitizer enabled")

      target_compile_options(${target_name} INTERFACE /fsanitize=address)
      target_link_options(${target_name} INTERFACE /fsanitize=address)
    else()
      message(STATUS "MSVC sanitizers disabled (only ASan supported)")
    endif()

  else()
    message(WARNING "Sanitizers not supported for: ${CMAKE_CXX_COMPILER_ID}")
  endif()

endfunction()