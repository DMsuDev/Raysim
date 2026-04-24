# ================================================
# Compiler behavior / options configuration
# ================================================
include_guard()

# Global compiler behavior (toolchain-like)
if(MSVC)
  add_compile_options(
    /utf-8              # Set UTF-8 encoding for source files and diagnostics
    /permissive-        # Enforce standard C++ behavior
    /Zc:__cplusplus     # Ensure __cplusplus macro reflects the actual C++ standard version
    /Zc:preprocessor    # Use the standard preprocessor
  )
else()
  add_compile_options(
    -finput-charset=UTF-8   # Set UTF-8 encoding for source files
    -fexec-charset=UTF-8    # Set UTF-8 encoding for diagnostics
  )
endif()
