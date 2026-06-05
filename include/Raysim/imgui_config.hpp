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
// ImGui DLL visibility config  (imgui_config.hpp)
// ----------------------------------------------------------------------------
// Drop this file into any project and point IMGUI_USER_CONFIG at it.
//
// Define these macros BEFORE including any ImGui header (or via CMake):
//
//   IMGUI_BUILD_SHARED   -> defined in all targets that link ImGui as a DLL
//                           (both the DLL itself and every consumer).
//   IMGUI_BUILD_DLL      -> defined ONLY when compiling the ImGui DLL itself;
//                           triggers dllexport instead of dllimport on Windows.
//
// If neither is defined, ImGui is assumed to be linked statically and
// IMGUI_API is left empty (no-op), which is the default ImGui behavior.
// ============================================================================

#ifndef IMGUI_API
    #if defined(IMGUI_BUILD_SHARED)

        // ---- Windows / Cygwin ------------------------------------------------
        #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
            #if defined(IMGUI_BUILD_DLL)
                #define IMGUI_API __declspec(dllexport)
            #else
                #define IMGUI_API __declspec(dllimport)
            #endif

        // ---- GCC/Clang visible platforms (Linux, macOS, etc.) ----------------
        #elif defined(__GNUC__) || defined(__clang__)
            #define IMGUI_API __attribute__((visibility("default")))

        // ---- Unknown platform ------------------------------------------------
        #else
            #define IMGUI_API
        #endif

      #else
            // Static linking: IMGUI_API is intentionally empty
            #define IMGUI_API
      #endif
#endif  // IMGUI_API
