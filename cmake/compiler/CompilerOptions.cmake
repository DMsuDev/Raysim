# ================================================
# Compiler behavior / options configuration
# ================================================
include_guard()

function(set_project_options target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  set(MSVC_OPTIONS
    /permissive-        # Disable non-standard extensions (enforce standard compliance)
    /Zc:__cplusplus     # Make __cplusplus reflect actual C++ standard version
    /Zc:preprocessor    # Enable conforming preprocessor behavior
    /utf-8              # Assume source files are UTF-8 encoded
  )

  if(MSVC)
    set(PROJECT_OPTIONS ${MSVC_OPTIONS})
  endif()

  target_compile_options(${target_name} PRIVATE ${PROJECT_OPTIONS})

endfunction()
