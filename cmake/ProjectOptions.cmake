# Set default build type to Debug if not specified by the user and not using multi-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Build type: Debug, Release, RelWithDebInfo, MinSizeRel")
endif()

# Configure whether to build example executables
option(RAYSIM_BUILD_EXAMPLES "Build RaySim example executables" OFF)

# Configure whether to treat warnings as errors
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Configure whether to enable sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer, etc.)
option(ENABLE_SANITIZERS "Enable sanitizers for debugging (Address, Undefined Behavior, etc.)" OFF)
