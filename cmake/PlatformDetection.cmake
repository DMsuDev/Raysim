# == Raysim - Platform Detection CMake Module ==
# This module detects the platform and sets variables accordingly.

include_guard()

# Detect the platform and set variables accordingly
if(WIN32)
    set(RS_PLATFORM_WINDOWS ON)
elseif(APPLE)
    set(RS_PLATFORM_MACOS ON)
elseif(UNIX)
    set(RS_PLATFORM_LINUX ON)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
