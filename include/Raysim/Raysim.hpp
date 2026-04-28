/**********************************************************************************************
 *   Raysim - A simple 2D graphics engine
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

#define RAYSIM_VERSION_MAJOR 0
#define RAYSIM_VERSION_MINOR 6
#define RAYSIM_VERSION_PATCH 1
#define RAYSIM_VERSION "0.6.1"

// Function specifiers in case library is build/used as a shared library
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
// NOTE: visibility("default") attribute makes symbols "visible" when compiled with -fvisibility=hidden
#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(RS_BUILD_SHARED)
        #if defined(RS_BUILD_DLL)
            #define RS_API __declspec(dllexport)   // We are building the library as a Win32 shared library (.dll)
        #else
            #define RS_API __declspec(dllimport)   // We are using the library as a Win32 shared library (.dll)
        #endif
    #else
        // We are building/using the library as a static library (.lib), no special specifier needed
        #define RS_API
    #endif
#else
    #if defined(RS_BUILD_SHARED)
        #define RS_API __attribute__((visibility("default"))) // We are building as a Unix shared library (.so/.dylib)
    #else
        #define RS_API
    #endif
#endif

// Some compilers (mostly macos clang) default to C++98,
// where aggregate initialization can't be used
// So, give a more clear error stating how to fix this
#if __cplusplus < 201103L
    #error "Raysim requires C++11 or later. Please enable C++11 support in your compiler settings (e.g. -std=c++11)."
#endif

#include "Raysim/Core/Base.hpp"
#include "Raysim/Core/Application.hpp"

#include "Raysim/Graphics/Shapes.hpp"
#include "Raysim/Graphics/Texts.hpp"
#include "Raysim/Graphics/Color.hpp"

#include "Raysim/Math/Math.hpp"

#include "Raysim/Input/KeyCodes.hpp"
