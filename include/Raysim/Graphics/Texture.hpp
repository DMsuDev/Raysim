#pragma once

// TODO: Texture API is not yet integrated into the engine.
// Missing: RenderCommand::CreateTexture() factory, public exposure via Raysim.hpp,
// and scene-level usage. The abstract interface + RaylibTexture backend exist
// but are disconnected. Complete before exposing to users.
#pragma message("[Raysim] Texture.hpp: API is WIP and not yet integrated")

namespace RS::Gfx
{
    enum class TextureFormat
    {
        Unknown,
        R8,
        RG8,
        RGB8,
        RGBA8
    };

    struct TextureDesc
    {
        int width = 0;
        int height = 0;
        TextureFormat format = TextureFormat::RGBA8;
        bool generateMipmaps = false;
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;

        virtual void Bind(int slot) const = 0;
    };
}
