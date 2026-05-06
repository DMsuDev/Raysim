#include "pch.hpp"

#include "Backend/Raylib/RaylibImGuiBackend.hpp"
#include "rlImGui.h"

namespace RS::Backend {

void RaylibImGuiBackend::Init(Window& /*window*/)
{
    rlImGuiSetup(true);
    m_Initialized = true;
}

void RaylibImGuiBackend::Shutdown()
{
    if (!m_Initialized)
        return;

    rlImGuiShutdown();
    m_Initialized = false;
}

void RaylibImGuiBackend::BeginFrame()
{
    if (!m_Initialized)
        return;

    rlImGuiBegin();
}

void RaylibImGuiBackend::EndFrame()
{
    if (!m_Initialized)
        return;

    rlImGuiEnd();
}

} // namespace RS::Backend
