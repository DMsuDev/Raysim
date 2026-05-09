#include "pch.hpp"

#include "Backend/Raylib/RaylibFontRenderer.hpp"
#include "Raysim/Fonts/Types/FontTypes.hpp"

#include <raylib.h>
#include <cmath>

namespace RS::Backend {

using namespace RS::Fonts;

namespace {

// Convert RS::Color to raylib ::Color.
[[nodiscard]] constexpr ::Color ToRL(const Color& c) noexcept
{
    return ::Color{ c.r, c.g, c.b, c.a };
}

} // anonymous namespace

struct RaylibFontRenderer::AtlasTexture
{
    ::Texture2D texture{};
};

struct RaylibFontRenderer::DefaultFont
{
    ::Font font{};
};

void RaylibFontRenderer::AtlasTextureDeleter::operator()(AtlasTexture* ptr) const noexcept
{
    delete ptr;
}

void RaylibFontRenderer::DefaultFontDeleter::operator()(DefaultFont* ptr) const noexcept
{
    delete ptr;
}

RaylibFontRenderer::~RaylibFontRenderer()
{
    Shutdown();
}

// ============================================================================
// Lifecycle
// ============================================================================

void RaylibFontRenderer::Init()
{
    RS_PROFILE_FUNCTION();
    RS_CORE_DEBUG("RaylibFontRenderer: initialized");
}

void RaylibFontRenderer::Shutdown()
{
    RS_PROFILE_FUNCTION();
    size_t released = 0;
    for (auto& [handle, tex] : m_Textures) {
        if (tex && tex->texture.id > 0) {
            ::UnloadTexture(tex->texture);
            ++released;
        }
    }
    m_Textures.clear();
    m_DefaultFont.reset();
    m_DefaultFontReady = false;
    RS_CORE_DEBUG("RaylibFontRenderer: shut down ({} textures released)", released);
}

// ============================================================================
// Atlas management
// ============================================================================

bool RaylibFontRenderer::UploadFontAtlas(FontHandle handle, const FontAtlas& atlas)
{
    RS_PROFILE_FUNCTION();

    if (!atlas.IsValid()) {
        RS_CORE_ERROR("RaylibFontRenderer::UploadFontAtlas - invalid atlas for handle {}", handle);
        return false;
    }

    // -- Convert R8 grayscale to RGBA (white + alpha) ----------------------
    // We store white pixels with alpha=grayscale so that color tinting via
    // DrawTexturePro works correctly.
    const uint32_t pixelCount = atlas.width * atlas.height;
    std::vector<uint8_t> rgba;
    rgba.reserve(pixelCount * 4);

    for (uint32_t i = 0; i < pixelCount; ++i) {
        rgba.push_back(255);            // R
        rgba.push_back(255);            // G
        rgba.push_back(255);            // B
        rgba.push_back(atlas.data[i]);  // A (from grayscale atlas)
    }

    // -- Upload to GPU via raylib Image -> Texture2D -----------------------
    ::Image img{};
    img.data    = rgba.data();
    img.width   = static_cast<int>(atlas.width);
    img.height  = static_cast<int>(atlas.height);
    img.mipmaps = 1;
    img.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    ::Texture2D tex = ::LoadTextureFromImage(img);
    // NOTE: LoadTextureFromImage copies the data to GPU memory.
    // rgba can be freed after this call (it goes out of scope naturally).

    if (tex.id == 0) {
        RS_CORE_ERROR("RaylibFontRenderer: GPU texture upload failed for handle {}", handle);
        return false;
    }

    ::SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    auto& slot = m_Textures[handle];
    if (slot && slot->texture.id > 0)
        ::UnloadTexture(slot->texture);

    slot = AtlasTexturePtr(new AtlasTexture());
    slot->texture = tex;

    RS_CORE_DEBUG("RaylibFontRenderer: uploaded atlas for handle {} ({}x{}, texId={})",
                  handle, atlas.width, atlas.height, tex.id);
    return true;
}

void RaylibFontRenderer::ReleaseFontAtlas(FontHandle handle)
{
    const auto it = m_Textures.find(handle);
    if (it == m_Textures.end()) return;

    if (it->second && it->second->texture.id > 0)
        ::UnloadTexture(it->second->texture);

    m_Textures.erase(it);
    RS_CORE_DEBUG("RaylibFontRenderer: released atlas for handle {}", handle);
}

// ============================================================================
// Measurement
// ============================================================================

Math::Vec2f RaylibFontRenderer::MeasureDefaultText(const std::string& text, float fontSize) const
{
    const int w = ::MeasureText(text.c_str(), static_cast<int>(fontSize));
    return Math::Vec2f{ static_cast<float>(w), fontSize };
}

// ============================================================================
// Rendering
// ============================================================================

void RaylibFontRenderer::RenderText(
    FontHandle         handle,
    const Font&        font,
    const std::string& text,
    float              x,
    float              y,
    float              fontSize,
    float              spacing,
    const Color&       color)
{
    RS_ASSERT(fontSize > 0, "RaylibFontRenderer: fontSize must be positive (got {})", fontSize);

    const ::Color rlColor = ToRL(color);

    // -- Default font path (no atlas) --------------------------------------
    if (handle == DEFAULT_FONT_HANDLE || font.IsDefault()) {
        if (!m_DefaultFontReady) {
            m_DefaultFont      = DefaultFontPtr(new DefaultFont());
            m_DefaultFont->font = ::GetFontDefault();
            m_DefaultFontReady = true;
        }
        ::DrawTextEx(m_DefaultFont->font, text.c_str(),
                     ::Vector2{ std::round(x), std::round(y) },
                     fontSize, spacing, rlColor);
        return;
    }

    // -- Custom atlas path -------------------------------------------------
    const auto texIt = m_Textures.find(handle);
    if (texIt == m_Textures.end() || !texIt->second) {
        RS_CORE_WARN("RaylibFontRenderer: no texture for handle {} - was UploadFontAtlas called?", handle);
        return;
    }

    const ::Texture2D& tex = texIt->second->texture;

    // Scale factor: render size / rasterized size
    const float scale = fontSize / static_cast<float>(font.GetSize());

    // Cursor starts at the top-left of the text block (y_top convention).
    // stb_truetype's yoff is relative to the baseline, where yoff < 0 means
    // the glyph sits above the baseline (normal for uppercase/lowercase).
    // To convert y_top to baseline:  baseline = y_top + ascender * scale
    const float baselineY = std::round(y) + font.GetAscent() * scale;
    float cursorX = std::round(x);

    for (const char rawCh : text) {
        const uint32_t cp = static_cast<uint32_t>(static_cast<unsigned char>(rawCh));
        const Glyph* g = font.GetGlyph(cp);

        if (!g) {
            // Fallback: advance by a space-width if the glyph is missing
            const Glyph* space = font.GetGlyph(32u);
            if (space)
                cursorX += space->advanceX * scale + spacing;
            continue;
        }

        // Skip invisible glyphs (e.g. space - no atlas rect)
        if (g->atlasRegion.atlasSize.x > 0 && g->atlasRegion.atlasSize.y > 0) {
            // Source rect in the atlas texture
            const ::Rectangle src{
                static_cast<float>(g->atlasRegion.atlasPos.x),
                static_cast<float>(g->atlasRegion.atlasPos.y),
                static_cast<float>(g->atlasRegion.atlasSize.x),
                static_cast<float>(g->atlasRegion.atlasSize.y)
            };

            // Destination rect on screen (glyph top-left in screen space)
            // offset.x/offset.y: pixel offset from the baseline/cursor to the glyph top-left
            const ::Rectangle dst{
                cursorX + g->offset.x * scale,
                baselineY + g->offset.y * scale,
                g->atlasRegion.atlasSize.x * scale,
                g->atlasRegion.atlasSize.y * scale
            };

            ::DrawTexturePro(tex, src, dst,
                             ::Vector2{ 0.0f, 0.0f }, // origin (top-left of dst)
                             0.0f,                    // rotation
                             rlColor);
        }

        // Advance cursor
        cursorX += g->advanceX * scale + spacing;
    }
}

} // namespace RS
