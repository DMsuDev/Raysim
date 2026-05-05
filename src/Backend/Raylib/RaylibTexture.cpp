#include "pch.hpp"

#include "Backend/Raylib/RaylibTexture.hpp"

#include <raylib.h>
#include <rlgl.h>

#include <cstring>
#include <limits>

namespace RS::Backend {

namespace {

constexpr int ToRaylibFormat(Gfx::TextureFormat format) noexcept
{
    switch (format) {
        case Gfx::TextureFormat::R8:    return PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        case Gfx::TextureFormat::RG8:   return PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
        case Gfx::TextureFormat::RGB8:  return PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        case Gfx::TextureFormat::RGBA8: return PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        case Gfx::TextureFormat::Unknown:
        default:                        return PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    }
}

constexpr int BytesPerPixel(Gfx::TextureFormat format) noexcept
{
    switch (format) {
        case Gfx::TextureFormat::R8:    return 1;
        case Gfx::TextureFormat::RG8:   return 2;
        case Gfx::TextureFormat::RGB8:  return 3;
        case Gfx::TextureFormat::RGBA8: return 4;
        case Gfx::TextureFormat::Unknown:
        default:                        return 4;
    }
}

} // namespace

struct RaylibTexture::TextureHandle
{
    ::Texture2D texture{};
};

RaylibTexture::RaylibTexture(const Gfx::TextureDesc& desc)
    : m_Desc(desc)
{
    if (m_Desc.width <= 0 || m_Desc.height <= 0) {
        RS_CORE_ERROR("RaylibTexture: invalid texture size {}x{}", m_Desc.width, m_Desc.height);
        m_Desc.width = 0;
        m_Desc.height = 0;
        return;
    }

    if (m_Desc.format == Gfx::TextureFormat::Unknown)
        m_Desc.format = Gfx::TextureFormat::RGBA8;

    const int bpp = BytesPerPixel(m_Desc.format);
    const size_t dataSize = static_cast<size_t>(m_Desc.width) *
                            static_cast<size_t>(m_Desc.height) *
                            static_cast<size_t>(bpp);

    ::Image img{};
    img.width = m_Desc.width;
    img.height = m_Desc.height;
    img.mipmaps = 1;
    img.format = ToRaylibFormat(m_Desc.format);

    const size_t maxAlloc = static_cast<size_t>(std::numeric_limits<unsigned int>::max());
    if (dataSize > maxAlloc) {
        RS_CORE_ERROR("RaylibTexture: allocation size too large ({} bytes)", dataSize);
        return;
    }

    img.data = ::MemAlloc(static_cast<unsigned int>(dataSize));
    if (!img.data) {
        RS_CORE_ERROR("RaylibTexture: allocation failed ({} bytes)", dataSize);
        return;
    }
    std::memset(img.data, 0, dataSize);

    ::Texture2D tex = ::LoadTextureFromImage(img);
    ::UnloadImage(img);

    if (tex.id == 0) {
        RS_CORE_ERROR("RaylibTexture: GPU texture creation failed");
        return;
    }

    if (m_Desc.generateMipmaps) {
        ::GenTextureMipmaps(&tex);
        ::SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
    } else {
        ::SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    }

    m_Handle = std::make_unique<TextureHandle>();
    m_Handle->texture = tex;
}

RaylibTexture::~RaylibTexture()
{
    if (m_Handle && m_Handle->texture.id > 0)
        ::UnloadTexture(m_Handle->texture);
}

int RaylibTexture::GetWidth() const
{
    return m_Desc.width;
}

int RaylibTexture::GetHeight() const
{
    return m_Desc.height;
}

Gfx::TextureFormat RaylibTexture::GetFormat() const
{
    return m_Desc.format;
}

void RaylibTexture::Bind(int slot) const
{
    if (!m_Handle || m_Handle->texture.id == 0)
        return;

    if (slot >= 0)
        ::rlActiveTextureSlot(slot);

    ::rlSetTexture(m_Handle->texture.id);
}

} // namespace RS::Backend
