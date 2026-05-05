#include "pch.hpp"

// stb_truetype: single-header library. We own the implementation translation unit.
// Place stb_truetype.h in:  third_party/stb/stb_truetype.h
// Source: https://github.com/nothings/stb/blob/master/stb_truetype.h  (MIT license)
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" // found via target_include_directories -> third_party/stb

#include "Raysim/Fonts/Providers/STBTrueTypeProvider.hpp"

#include <cstdio>
#include <cmath>
#include <algorithm>

namespace RS::Fonts {

// ============================================================================
// Helpers
// ============================================================================

namespace {

/// Read a binary file into a heap buffer.  Returns empty vector on failure.
std::vector<uint8_t> ReadFile(const std::string& path)
{
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f)
        return {};

    std::fseek(f, 0, SEEK_END);
    const long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    if (size <= 0) {
        std::fclose(f);
        return {};
    }

    std::vector<uint8_t> buf(static_cast<size_t>(size));
    std::fread(buf.data(), 1, static_cast<size_t>(size), f);
    std::fclose(f);
    return buf;
}

} // anonymous namespace

// ============================================================================
// STBTrueTypeProvider - public interface
// ============================================================================

void STBTrueTypeProvider::ChooseAtlasSize(int fontSize,
                                          uint32_t& outW, uint32_t& outH) noexcept
{
    if (fontSize <= 48)       { outW = outH = 512;  }
    else if (fontSize <= 96)  { outW = outH = 1024; }
    else                      { outW = outH = 2048; }
}

// ----------------------------------------------------------------------------

bool STBTrueTypeProvider::LoadFont(
    const std::string& path,
    int                fontSize,
    uint32_t           firstChar,
    uint32_t           charCount,
    Font&              outFont)
{
    RS_CORE_INFO("STBTrueTypeProvider: loading '{}' @ {}px ({} glyphs)", path, fontSize, charCount);

    // Read the entire TTF file into memory.
    std::vector<uint8_t> ttfBuf = ReadFile(path);
    if (ttfBuf.empty()) {
        RS_CORE_ERROR("STBTrueTypeProvider: could not open '{}'", path);
        return false;
    }

    // Validate and parse the font file.
    stbtt_fontinfo info{};
    if (!stbtt_InitFont(&info, ttfBuf.data(), 0)) {
        RS_CORE_ERROR("STBTrueTypeProvider: invalid or unsupported font '{}'", path);
        return false;
    }

    // Choose atlas dimensions based on fontSize heuristics.
    uint32_t atlasW = 0, atlasH = 0;
    ChooseAtlasSize(fontSize, atlasW, atlasH);

    // Pack the requested character range into the atlas bitmap. The resulting
    // packedChars array contains the pixel coordinates and metrics for each glyph.
    std::vector<uint8_t> pixels(atlasW * atlasH, 0);
    std::vector<stbtt_packedchar> packedChars(charCount);

    stbtt_pack_context ctx{};
    if (!stbtt_PackBegin(&ctx, pixels.data(),
                         static_cast<int>(atlasW),
                         static_cast<int>(atlasH),
                         /*stride=*/0, /*padding=*/1, nullptr))
    {
        RS_CORE_ERROR("STBTrueTypeProvider: atlas PackBegin failed for '{}'", path);
        return false;
    }

    if (!stbtt_PackFontRange(&ctx, ttfBuf.data(), /*fontIndex=*/0,
                             static_cast<float>(fontSize),
                             static_cast<int>(firstChar),
                             static_cast<int>(charCount),
                             packedChars.data()))
    {
        stbtt_PackEnd(&ctx);
        RS_CORE_ERROR("STBTrueTypeProvider: atlas PackFontRange failed - try a larger atlas (fontSize={})", fontSize);
        return false;
    }

    stbtt_PackEnd(&ctx);

    // Calculate font metrics (ascender, descender, line gap) at the chosen pixel height.
    const float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(fontSize));

    int ascender = 0, descender = 0, lineGap = 0;
    stbtt_GetFontVMetrics(&info, &ascender, &descender, &lineGap);

    const float ascenderPx   =  static_cast<float>(ascender)  * scale;
    const float descenderPx  =  static_cast<float>(descender) * scale;
    const float lineHeightPx =  static_cast<float>(ascender - descender + lineGap) * scale;

    // Construct the FontAtlas and populate glyph metrics based on the packed character data.
    FontAtlas atlas;
    atlas.width  = static_cast<uint16_t>(atlasW);
    atlas.height = static_cast<uint16_t>(atlasH);

    atlas.data = std::move(pixels);

    atlas.metrics.lineHeight = lineHeightPx;
    atlas.metrics.ascent     = ascenderPx;
    atlas.metrics.descent    = descenderPx;
    atlas.glyphs.reserve(charCount);
    atlas.codepointToIndex.assign(firstChar + charCount, INVALID_GLYPH);

    for (uint32_t i = 0; i < charCount; ++i) {
        const uint32_t           cp = firstChar + i;
        const stbtt_packedchar& pc = packedChars[i];

        const uint16_t glW = static_cast<uint16_t>(pc.x1 - pc.x0);
        const uint16_t glH = static_cast<uint16_t>(pc.y1 - pc.y0);

        Glyph g;
        g.codepoint = cp;
        g.atlasRegion.atlasPos  = {static_cast<uint16_t>(pc.x0), static_cast<uint16_t>(pc.y0)};
        g.atlasRegion.atlasSize = {glW, glH};

        g.offset.x = pc.xoff;
        g.offset.y = pc.yoff;
        g.advanceX = pc.xadvance;

        // UVs are normalized [0,1] based on atlas dimensions
        g.uvRect.uvMin = {static_cast<float>(pc.x0) / static_cast<float>(atlasW),
                          static_cast<float>(pc.y0) / static_cast<float>(atlasH)};
        g.uvRect.uvMax = {static_cast<float>(pc.x1) / static_cast<float>(atlasW),
                          static_cast<float>(pc.y1) / static_cast<float>(atlasH)};

        atlas.codepointToIndex[cp] = i;
        atlas.glyphs.push_back(g);
    }

    // Populate output Font struct with metadata and atlas.
    outFont.SetPath(path);
    outFont.SetSize(fontSize);
    outFont.SetAtlas(std::move(atlas));
    outFont.SetLoaded(true);
    outFont.SetDefault(false);

    RS_CORE_INFO("STBTrueTypeProvider: loaded '{}' - atlas {}x{}, {} glyphs, lineHeight={:.1f}px",
                 path, atlasW, atlasH, charCount, lineHeightPx);
    return true;
}

Math::Vec2 STBTrueTypeProvider::MeasureText(
    const Font&        font,
    const std::string& text,
    float              spacing) const
{
    if (text.empty() || !font.IsLoaded())
        return {0.0f, 0.0f};

    float width = 0.0f;
    bool  first = true;

    for (const char raw_ch : text) {
        const Glyph* glyph = font.GetGlyph(static_cast<uint32_t>(static_cast<unsigned char>(raw_ch)));
        if (!glyph)
            continue;

        if (!first)
            width += spacing;

        width += glyph->advanceX;
        first = false;
    }

    return Math::Vec2{ width, font.GetLineHeight() };
}

} // namespace RS::Fonts
