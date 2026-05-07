#pragma once

#include "Raysim/Core/Base.hpp"
#include "Raysim/Fonts/Rendering/FontRenderer.hpp"
#include "Raysim/Fonts/Types/FontTypes.hpp"

#include <unordered_map>

namespace RS::Backend {

class RaylibFontRenderer final : public Fonts::FontRenderer
{
public:
    RaylibFontRenderer() = default;
    RaylibFontRenderer(const RaylibFontRenderer&) = delete;

    ~RaylibFontRenderer() override;
    RaylibFontRenderer& operator=(const RaylibFontRenderer&) = delete;

    // FontRenderer interface

    void Init()     override;
    void Shutdown() override;

    bool UploadFontAtlas(Fonts::FontHandle handle, const Fonts::FontAtlas& atlas) override;
    void ReleaseFontAtlas(Fonts::FontHandle handle)                        override;

    void RenderText(
        Fonts::FontHandle         handle,
        const Fonts::Font&        font,
        const std::string& text,
        float              x,
        float              y,
        float              fontSize,
        float              spacing,
        const Color&       color) override;

    Math::Vec2 MeasureDefaultText(const std::string& text, float fontSize) const override;

private:
    struct AtlasTexture;
    struct DefaultFont;

    struct AtlasTextureDeleter { void operator()(AtlasTexture* ptr) const noexcept; };
    struct DefaultFontDeleter { void operator()(DefaultFont* ptr) const noexcept; };

    using AtlasTexturePtr = std::unique_ptr<AtlasTexture, AtlasTextureDeleter>;
    using DefaultFontPtr = std::unique_ptr<DefaultFont, DefaultFontDeleter>;

    std::unordered_map<Fonts::FontHandle, AtlasTexturePtr> m_Textures;

    /// Cached raylib default font (filled on first use).
    DefaultFontPtr m_DefaultFont;
    bool   m_DefaultFontReady = false;
};

} // namespace RS::Backend
