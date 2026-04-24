#pragma once

/**
 * Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Main headers for the Raysim framework

// Exclude conflicting Win32 definitions that clash with raylib
// (wingdi.h defines Rectangle as a function, winuser.h defines CloseWindow/ShowCursor)
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef NOUSER
        #define NOUSER
    #endif
    #undef DrawTextEx
    #undef DrawText
    #undef CreateWindow
#endif

#include "Raysim/Core/Base.hpp"

#include "Raysim/Graphics/Shapes.hpp"
#include "Raysim/Graphics/Texts.hpp"
#include "Raysim/Graphics/Color.hpp"

#include "Raysim/Math/Math.hpp"
#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Math/Vector2Int.hpp"
#include "Raysim/Math/Vector3.hpp"
#include "Raysim/Math/Vector3Int.hpp"
#include "Raysim/Math/Random.hpp"

#include "Raysim/Input/KeyCodes.hpp"

#include "Raysim/Core/ApplicationConfig.hpp"
#include "Raysim/Core/Application.hpp"

#define RAYSIM_VERSION_MAJOR 0
#define RAYSIM_VERSION_MINOR 6
#define RAYSIM_VERSION_PATCH 0
#define RAYSIM_VERSION "0.6.0"
