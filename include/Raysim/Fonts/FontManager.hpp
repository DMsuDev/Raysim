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

#include "Types/Font.hpp"
#include "Types/FontTypes.hpp"
#include "Rendering/FontRenderer.hpp"
#include "Providers/TrueTypeProvider.hpp"

#include "Raysim/Core/Base.hpp"
#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace RS::Fonts {

class FontManager {
public:
    FontManager() = delete; // Static-only

    // ================================================================
    // Initialization
    // ================================================================

    /**
     * @brief Set the TTF parsing backend.
     * Must be called before any LoadFont() call.
     * @param provider Owning pointer to the provider (e.g. STBTrueTypeProvider).
     */
    static void SetProvider(Scope<TrueTypeProvider> provider);

    /**
     * @brief Set the GPU rendering backend.
     * Must be called before any LoadFont() or RenderText() call.
     * @param renderer Owning pointer to the renderer (e.g. RaylibFontRenderer).
     */
    static void SetRenderer(Scope<FontRenderer> renderer);

    /// Release all loaded fonts and shut down both subsystems.
    static void Shutdown();

    // ================================================================
    // Loading
    // ================================================================

    /**
     * @brief Load a TTF/OTF font from disk and upload its atlas to the GPU.
     *
     * If a font with the same @p name is already loaded, the existing handle
     * is returned without reloading (cached).
     *
     * @param name       Logical identifier (e.g. "ui", "title").
     * @param path       Filesystem path relative to the application directory.
     * @param fontSize   Rasterization size in pixels.
     * @param firstChar  First Unicode codepoint (default: 32 = space).
     * @param charCount  Number of consecutive codepoints (default: 95 = printable ASCII).
     * @return Valid FontHandle on success, INVALID_FONT_HANDLE on failure.
     */
    static FontHandle LoadFont(
        const std::string& name,
        const std::string& path,
        int                fontSize,
        uint32_t           firstChar = 32,
        uint32_t           charCount = 95);

    /**
     * @brief Register the engine built-in fallback font.
     *
     * The default font uses the backend's own built-in glyph data (e.g.
     * raylib's embedded font). No TTF file is required.
     *
     * @return DEFAULT_FONT_HANDLE (always valid).
     */
    static FontHandle LoadDefaultFont();

    // ================================================================
    // Getters
    // ================================================================

    /// @return Pointer to the Font for @p handle, or nullptr if not found.
    static const Font* GetFont(FontHandle handle);

    /// @return Pointer to the Font with logical name @p name, or nullptr.
    static const Font* GetFontByName(const std::string& name);

    /// @return Handle for the font registered under @p name, or INVALID_FONT_HANDLE.
    static FontHandle  GetFontHandle(const std::string& name);

    /// @return The handle registered as "default", or DEFAULT_FONT_HANDLE if none was loaded.
    static FontHandle  GetDefaultHandle();

    /// @return true if @p handle is valid and has a loaded Font.
    static bool        HasFont(FontHandle handle);

    /// @return true if a font with logical name @p name is loaded.
    static bool        HasFont(const std::string& name);

    /// @return Sorted list of all loaded font names (for debug/tooling).
    static std::vector<std::string> GetLoadedFontNames();

    // ================================================================
    // Unloading
    // ================================================================

    /// Unload font by handle (releases GPU texture and CPU atlas).
    static void UnloadFont(FontHandle handle);

    /// Unload font by logical name.
    static void UnloadFont(const std::string& name);

    /// Unload every loaded font.
    static void UnloadAll();

    // ================================================================
    // Metrics
    // ================================================================

    /**
     * @brief Measure text dimensions using the loaded font's metrics.
     *
     * @param handle   Font handle (uses default if INVALID_FONT_HANDLE).
     * @param text     String to measure.
     * @param spacing  Extra pixels between characters (default 0).
     * @return Tight bounding box as Vec2{width, height}.
     */
    static Math::Vec2f MeasureText(
        FontHandle         handle,
        const std::string& text,
        float              spacing = 0.0f);

    // ================================================================
    // Rendering
    // ================================================================

    /**
     * @brief Draw text using the specified font.
     *
     * @param handle    Font handle.
     * @param text      String to draw.
     * @param x         Screen X (top-left of text).
     * @param y         Screen Y (top-left of text).
     * @param fontSize  Render size in pixels.
     * @param spacing   Extra pixels between characters.
     * @param color     Text tint color.
     */
    static void RenderText(
        FontHandle         handle,
        const std::string& text,
        float              x,
        float              y,
        float              fontSize,
        float              spacing,
        const Color&       color);

private:
    // ================================================================
    // Internal types
    // ================================================================

    struct FontEntry
    {
        Font       font;
        FontHandle handle = INVALID_FONT_HANDLE;
    };

    // ================================================================
    // State
    // ================================================================

    static Scope<TrueTypeProvider> s_TextProvider;
    static Scope<FontRenderer> s_TextRenderer;

    static std::unordered_map<FontHandle, FontEntry> s_Fonts;
    static std::unordered_map<std::string, FontHandle> s_NameMap;

    static FontHandle s_DefaultHandle;
    static FontHandle s_NextHandle;

// ================================================================
// Helpers
// ================================================================

    static FontHandle GenerateHandle() noexcept;
};

} // namespace RS::Fonts
