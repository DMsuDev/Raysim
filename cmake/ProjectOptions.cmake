# ================================================
# ProjectOptions.cmake
# Centralized project options
# ================================================

include_guard()

# Global options
option(RS_ENABLE_SANITIZERS "Enable all sanitizers" OFF)
option(RS_ENABLE_COVERAGE   "Enable coverage reporting" OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Sanitizer options (if sanitizers are enabled)
option(RS_ENABLE_ASAN  "Enable AddressSanitizer" ON)
option(RS_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)
option(RS_ENABLE_TSAN  "Enable ThreadSanitizer" OFF)
option(RS_ENABLE_MSAN  "Enable MemorySanitizer (Clang only)" OFF)
option(RS_ENABLE_LSAN  "Enable LeakSanitizer (standalone)" OFF)

# Link Time Optimization
option(RS_ENABLE_LTO "Enable Link Time Optimization" OFF)

# Build examples
option(RS_BUILD_EXAMPLES "Build example applications" ON)

# Define RS_BUILD_DEBUG macro automatically in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(RS_BUILD_DEBUG)
endif()
