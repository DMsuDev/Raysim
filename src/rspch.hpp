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

//----------------------------------------------------------------------------------
// Exclude conflicting Win32 definitions
//----------------------------------------------------------------------------------
#if defined(_WIN32)
    // Reduce Windows.h footprint
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef NOGDI // excludes GDI macros: DrawText, DrawTextEx, etc.
        #define NOGDI
    #endif
    #ifndef NOUSER // excludes USER macros: CreateWindow, etc.
        #define NOUSER
    #endif

    // Safety net: undefine only if already defined by a previously included
    // windows.h (e.g. pulled in transitively by a third-party header).
    #ifdef DrawTextEx
        #undef DrawTextEx
    #endif
    #ifdef DrawText
        #undef DrawText
    #endif
    #ifdef CreateWindow
        #undef CreateWindow
    #endif
#endif

#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
#endif

// Standard library common includes for implementation files
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// Useful includes of Raysim

#include "Raysim/Core/Base.hpp"
