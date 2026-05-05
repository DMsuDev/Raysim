#pragma once

//----------------------------------------------------------------------------------
// Exclude conflicting Win32 definitions
//----------------------------------------------------------------------------------
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
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
