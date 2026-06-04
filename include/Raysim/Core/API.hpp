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
#include "Raysim/Core/PlatformDetection.hpp"

// ============================================================================
// RS_API - DLL export / import visibility macro
//
// Usage:
//   class RS_API MyClass { ... };
//   RS_API void MyFunction();
//
// Build-time control (set by CMake):
//   RS_BUILD_SHARED  - defined when Raysim is built as a shared library
//   RS_BUILD_DLL     - defined only when compiling the library itself (export)
//                      not defined in consumer code (import)
// ============================================================================

#if defined(RS_PLATFORM_WINDOWS)
    #if defined(RS_BUILD_SHARED)
        #if defined(RS_BUILD_DLL)
            #define RS_API __declspec(dllexport)
        #else
            #define RS_API __declspec(dllimport)
        #endif
    #else
        #define RS_API
    #endif
#else
    #if defined(RS_BUILD_SHARED)
        #define RS_API __attribute__((visibility("default")))
    #else
        #define RS_API
    #endif
#endif
