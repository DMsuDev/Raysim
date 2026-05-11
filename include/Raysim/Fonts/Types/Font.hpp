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

#include "FontAtlas.hpp"

#include <string>

namespace RS::Fonts {

// High-level font class that owns the atlas and provides glyph lookup.
class Font {

public:
    const std::string& GetName() const { return m_Name; }
    const std::string& GetPath() const { return m_Path; }
    int GetSize() const { return m_Size; }

    [[nodiscard]] const Glyph* GetGlyph(uint32_t codepoint) const noexcept {
        if (m_Atlas.IsValid()) {
            const Glyph* glyph = m_Atlas.GetGlyph(codepoint);
            if (glyph)
                return glyph;
        }
        return m_Fallback; // Return fallback glyph if not found
    }

    // --- Font metrics ---
    [[nodiscard]] float GetAscent()     const noexcept { return m_Atlas.metrics.ascent; }
    [[nodiscard]] float GetDescent()    const noexcept { return m_Atlas.metrics.descent; }
    [[nodiscard]] float GetLineGap()    const noexcept { return m_Atlas.metrics.lineGap; }
    [[nodiscard]] float GetLineHeight() const noexcept { return m_Atlas.metrics.lineHeight; }

    [[nodiscard]] const FontAtlas& GetAtlas() const noexcept { return m_Atlas; }

    [[nodiscard]] bool IsLoaded()   const noexcept { return m_IsLoaded; }
    [[nodiscard]] bool IsDefault()  const noexcept { return m_IsDefault; }

    void SetName(const std::string& name)   { m_Name = name; }
    void SetPath(const std::string& path)   { m_Path = path; }
    void SetSize(int size)                  { m_Size = size; }
    void SetAtlas(FontAtlas atlas)          { m_Atlas = std::move(atlas); }
    void SetLoaded(bool loaded)             { m_IsLoaded = loaded; }
    void SetDefault(bool isDefault)         { m_IsDefault = isDefault; }
    void SetFallback(const Glyph* fallback) { m_Fallback = fallback; }

private:
    std::string m_Name;
    std::string m_Path;
    int m_Size = 0;

    FontAtlas m_Atlas;

    const Glyph* m_Fallback = nullptr;

    bool m_IsLoaded  = false;
    bool m_IsDefault = false;
};

} // namespace RS::Fonts
