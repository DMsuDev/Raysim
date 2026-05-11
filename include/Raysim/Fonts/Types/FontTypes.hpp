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

#include <cstdint>

namespace RS::Fonts {

// --- Font Handle ---

using FontHandle = uint32_t;

// Sentinel value representing an invalid or failed font load.
constexpr FontHandle INVALID_FONT_HANDLE = 0;

// Special handle used internally for the engine default font (backend built-in).
constexpr FontHandle DEFAULT_FONT_HANDLE = 1;

// --- Glyph Index ---

using GlyphIndex = uint32_t;

// Sentinel value representing an invalid glyph index (e.g. for unmapped codepoints).
constexpr GlyphIndex INVALID_GLYPH = 0xFFFFFFFF;

} // namespace RS::Fonts
