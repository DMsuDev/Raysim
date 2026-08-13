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

// Some compilers (mostly macos clang) default to C++98,
// where aggregate initialization can't be used
// So, give a more clear error stating how to fix this
#if __cplusplus < 201103L
    #error "Raysim requires C++11 or later. Please enable C++11 support in your compiler settings (e.g. -std=c++11)."
#endif

#include "Raysim/Version.hpp"

#include "Raysim/Core/Base.hpp"

#include "Raysim/Events.hpp"
#include "Raysim/Inputs.hpp"

#include "Raysim/Core/Window.hpp"
#include "Raysim/Core/Time.hpp"

#include "Raysim/Core/Application.hpp"

#include "Raysim/Graphics.hpp"
#include "Raysim/Scenes.hpp"
#include "Raysim/Fonts.hpp"
#include "Raysim/Layers.hpp"
#include "Raysim/Math.hpp"

namespace RS {
    using FontManager = RS::Fonts::FontManager;
} // namespace RS
