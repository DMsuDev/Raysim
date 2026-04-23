#pragma once

// ======================================================
// Platform Detection
// ======================================================

// Fallback: detect from compiler macros if CMake didn't define anything
#if !defined(RS_PLATFORM_WINDOWS) && !defined(RS_PLATFORM_LINUX)
    #if defined(_WIN32) || defined(_WIN64)
        #define RS_PLATFORM_WINDOWS
    #elif defined(__linux__)
        #define RS_PLATFORM_LINUX
    #else
        #error "Unsupported platform. Only Windows and Linux (x64) are supported."
    #endif
#endif

// ======================================================
// Platform Validation
// ======================================================

#if defined(RS_PLATFORM_WINDOWS)
    #if !defined(_WIN64)
        #error "Only x64 is supported on Windows."
    #endif
    #define RS_PLATFORM_NAME "Windows"

#elif defined(RS_PLATFORM_LINUX)
    #if defined(__i386__) || defined(__i686__)
        #error "Only x86_64 is supported on Linux."
    #endif
    #define RS_PLATFORM_NAME "Linux"

#else
    #error "Unsupported platform. Only Windows and Linux (x64) are supported."
#endif
