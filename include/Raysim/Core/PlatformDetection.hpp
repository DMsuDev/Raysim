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
