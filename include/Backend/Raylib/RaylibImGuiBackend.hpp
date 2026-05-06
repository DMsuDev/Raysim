#pragma once

#include "Raysim/ImGui/ImGuiBackend.hpp"

namespace RS { class Window; }

namespace RS::Backend {

class RaylibImGuiBackend final : public ImGuiBackend
{
public:
    RaylibImGuiBackend()  = default;
    ~RaylibImGuiBackend() override = default;

    void Init(Window& window) override;
    void Shutdown() override;
    void BeginFrame() override;
    void EndFrame() override;

private:
    bool m_Initialized = false;
};

} // namespace RS::Backend
