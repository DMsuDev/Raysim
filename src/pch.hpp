#pragma once

// Exclude conflicting Win32 definitions that clash with raylib
// (wingdi.h defines Rectangle as a function, winuser.h defines CloseWindow/ShowCursor)
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
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
#include <array>
#include <vector>
#include <unordered_map>

// Useful includes of Raysim

#include "Raysim/Core/Base.hpp"
