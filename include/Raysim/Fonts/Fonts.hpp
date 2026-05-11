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
// Raysim Font System - convenience aggregator header
//
// Include this single header to get access to the full font subsystem:
//   #include "Raysim/Fonts/Fonts.hpp"
// ============================================================================

#include "Raysim/Fonts/Types/FontTypes.hpp"
#include "Raysim/Fonts/Types/Glyph.hpp"
#include "Raysim/Fonts/Types/FontAtlas.hpp"
#include "Raysim/Fonts/Types/Font.hpp"
#include "Raysim/Fonts/Providers/TrueTypeProvider.hpp"
#include "Raysim/Fonts/Rendering/FontRenderer.hpp"
#include "Raysim/Fonts/Providers/STBTrueTypeProvider.hpp"
#include "Raysim/Fonts/FontManager.hpp"
