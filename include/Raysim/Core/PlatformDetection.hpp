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

// ============================================================================
// Platform Detection
// ----------------------------------------------------------------------------
// Preferred path: CMake defines RS_PLATFORM_* via target_compile_definitions,
// which means the detection below is skipped entirely in normal builds.
//
// Fallback path: if CMake didn't inject any platform macro (e.g. the header
// is used outside the CMake build system), we derive the platform from
// standard compiler-predefined macros. This keeps the header self-contained
// and usable in third-party projects that include it directly.
// ============================================================================

#if !defined(RS_PLATFORM_WINDOWS) && !defined(RS_PLATFORM_LINUX) && !defined(RS_PLATFORM_MACOS)

    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
        #define RS_PLATFORM_WINDOWS

    #elif defined(__APPLE__) && defined(__MACH__)
        #define RS_PLATFORM_MACOS

    #elif defined(__linux__)
        #define RS_PLATFORM_LINUX

    #else
        #error "[Raysim] Unsupported platform. Supported targets: Windows x64, Linux x86_64, macOS (experimental)."
    #endif

#endif // !defined(RS_PLATFORM_*)

// ============================================================================
// Platform Validation
// ----------------------------------------------------------------------------
// Enforces architecture and toolchain requirements per supported platform.
// Emits a compile-time error with a clear message rather than producing a
// binary that would silently misbehave at runtime.
//
// RS_PLATFORM_NAME is defined here so that diagnostic and logging code can
// embed the platform string without repeating these preprocessor checks.
// ============================================================================

#if defined(RS_PLATFORM_WINDOWS)

    #if !defined(_WIN64)
        #error "[Raysim] Only x64 (64-bit) is supported on Windows. 32-bit builds are not supported."
    #endif

    #define RS_PLATFORM_NAME "Windows"

#elif defined(RS_PLATFORM_MACOS)

    // macOS support is experimental: the Raylib backend and Metal/OpenGL
    // abstraction layer have not been fully validated on Apple Silicon or
    // Intel Macs.  Expect rough edges and please report issues at:
    // https://github.com/DMsuDev/Raysim/issues
    #pragma message("[Raysim] macOS support is experimental and may be unstable. Use at your own risk.")

    #if !defined(__x86_64__) && !defined(__arm64__) && !defined(__aarch64__)
        #error "[Raysim] macOS is only supported on x86_64 and Apple Silicon (arm64)."
    #endif

    #define RS_PLATFORM_NAME "macOS (experimental)"

#elif defined(RS_PLATFORM_LINUX)

    #if defined(__i386__) || defined(__i686__)
        #error "[Raysim] Only x86_64 is supported on Linux. 32-bit builds are not supported."
    #endif

    #define RS_PLATFORM_NAME "Linux"

#else

    #error "[Raysim] Unsupported platform. Supported targets: Windows x64, Linux x86_64, macOS (experimental)."

#endif // RS_PLATFORM_*
