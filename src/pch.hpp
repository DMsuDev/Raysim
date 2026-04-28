#pragma once

// Exclude conflicting Win32 definitions that clash with raylib and spdlog (e.g. min/max macros, DrawText, CreateWindow, etc.)
// (wingdi.h defines Rectangle as a function, winuser.h defines CloseWindow/ShowCursor)
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        // See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
        #define NOMINMAX
    #endif
    #define NOGDI
    #define NOUSER
    #undef DrawTextEx
    #undef DrawText
    #undef CreateWindow
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
