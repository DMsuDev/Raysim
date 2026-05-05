#pragma once

// TODO: See Texture.hpp this backend implementation is complete but unused.
// Wire up via BackendFactory and RenderCommand before shipping and scene-level usage.
#pragma message("[Raysim] RaylibTexture.hpp: backend impl is WIP and not yet integrated")

#include "Raysim/Core/Base.hpp"
#include "Raysim/Graphics/Texture.hpp"

namespace RS::Backend {

class RaylibTexture final : public Gfx::Texture
{
public:
    explicit RaylibTexture(const Gfx::TextureDesc& desc);
    ~RaylibTexture() override;

    RaylibTexture(const RaylibTexture&) = delete;
    RaylibTexture& operator=(const RaylibTexture&) = delete;
    RaylibTexture(RaylibTexture&&) noexcept = default;
    RaylibTexture& operator=(RaylibTexture&&) noexcept = default;

    int GetWidth() const override;
    int GetHeight() const override;
    Gfx::TextureFormat GetFormat() const override;

    void Bind(int slot) const override;

private:
    struct TextureHandle;

    Scope<TextureHandle> m_Handle;
    Gfx::TextureDesc m_Desc;
};

} // namespace RS::Backend
