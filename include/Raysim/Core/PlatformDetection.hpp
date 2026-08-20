/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

/**
 * @file PlatformDetection.hpp
 * @brief Platform detection macros for Raysim.
 *
 * This header file defines macros to detect the target platform (Windows, Linux, macOS)
 * and provides platform-specific configurations and validations.
 */

// ============================================================================
// Platform Detection
// ============================================================================

/**
 * @name Platform Detection Macros
 * @brief Macros to detect the target platform.
 *
 * The following macros are defined based on the detected platform:
 * - `RS_PLATFORM_WINDOWS`
 * - `RS_PLATFORM_LINUX`
 * - `RS_PLATFORM_MACOS`
 *
 * If none of the platform macros are predefined, the target platform is detected
 * automatically using compiler-provided platform macros. An error is raised if
 * the detected platform is unsupported.
 *
 * @{
 */

#if !defined(RS_PLATFORM_WINDOWS) && \
    !defined(RS_PLATFORM_LINUX)   && \
    !defined(RS_PLATFORM_MACOS)

    #if defined(_WIN32) || defined(__CYGWIN__)
        #define RS_PLATFORM_WINDOWS 1

        // Reduce the amount of declarations pulled in by <Windows.h> and prevent
        // the min/max macros from polluting the global namespace.
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN 1
        #endif
        #ifndef NOMINMAX
            #define NOMINMAX 1
        #endif

        // Exclude GDI declarations from <Windows.h> to prevent name conflicts
        // with Raysim's public API (e.g. DrawText, DrawTextEx).
        #ifndef NOGDI
            #define NOGDI 1
        #endif

        // Exclude USER declarations from <Windows.h> to prevent name conflicts
        // with Raysim's public API (e.g. CreateWindow, CreateWindowEx).
        #ifndef NOUSER
            #define NOUSER 1
        #endif

    #elif defined(__APPLE__) && defined(__MACH__)
        #define RS_PLATFORM_MACOS 1

    #elif defined(__linux__)
        #define RS_PLATFORM_LINUX 1

    #else
        #error "[Raysim] Unsupported platform. Supported targets: Windows x64, Linux x86_64, macOS (experimental)."
    #endif

#endif

/** @} */

// ============================================================================
// Platform Metadata
// ============================================================================

/// @brief Guards against conflicting platform macros supplied by the build
/// system (e.g. two RS_PLATFORM_* defines passed simultaneously via CMake).
#if (defined(RS_PLATFORM_WINDOWS) + \
     defined(RS_PLATFORM_LINUX)   + \
     defined(RS_PLATFORM_MACOS)) > 1
    #error "[Raysim] Multiple target platforms defined simultaneously."
#endif

/// @def RS_PLATFORM_NAME
/// @brief A string literal identifying the target platform at compile time.

#if defined(RS_PLATFORM_WINDOWS)
    #define RS_PLATFORM_NAME "Windows"

#elif defined(RS_PLATFORM_MACOS)
    #define RS_PLATFORM_NAME "macOS"

#elif defined(RS_PLATFORM_LINUX)
    #define RS_PLATFORM_NAME "Linux"

#else
    #error "[Raysim] Target platform is not supported. Raysim currently supports Windows, Linux and macOS only."
#endif

// ============================================================================
// Platform-Specific Validations
// ============================================================================

/**
 * @name Platform-Specific Validations
 * @brief Validates platform-specific requirements and configurations.
 *
 * The following validations are performed based on the detected platform:
 * - Windows: Ensures that only x64 (64-bit) builds are supported.
 * - macOS: Ensures that only x86_64 and Apple Silicon (arm64) builds are supported.
 * - Linux: Ensures that only x86_64 builds are supported.
 *
 * If any of the validations fail, a compile-time error is raised with a descriptive message.
 *
 * @{
 */
#if defined(RS_PLATFORM_WINDOWS)

    #if !defined(_WIN64)
        #error "[Raysim] Only x64 (64-bit) is supported on Windows. 32-bit builds are not supported."
    #endif

#elif defined(RS_PLATFORM_MACOS)

    // macOS support is experimental: the Raylib backend and the OpenGL
    // abstraction layer have not been fully validated on Apple Silicon or
    // Intel Macs. Expect rough edges and please report issues at:
    // https://github.com/DMsuDev/Raysim/issues
    #pragma message("[Raysim] macOS support is experimental and may be unstable. Use at your own risk.")

    #if !defined(__x86_64__) && !defined(__arm64__) && !defined(__aarch64__)
        #error "[Raysim] macOS is only supported on x86_64 and Apple Silicon (arm64)."
    #endif

#elif defined(RS_PLATFORM_LINUX)

    #if !defined(__x86_64__)
        #error "[Raysim] Only x86_64 is supported on Linux. 32-bit builds are not supported."
    #endif

#else
    #error "[Raysim] Unsupported platform. Supported targets: Windows x64, Linux x86_64, macOS (experimental)."
#endif

/** @} */
