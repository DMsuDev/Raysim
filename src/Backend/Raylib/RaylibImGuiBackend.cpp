#include "rspch.hpp"

#include "Backend/Raylib/RaylibImGuiBackend.hpp"
#include "rlImGui.h"

namespace RS::Backend {

void RaylibImGuiBackend::Init(Window& /*window*/)
{
    RS_PROFILE_FUNCTION();
    RS_CORE_ASSERT(!m_Initialized, "RaylibImGuiBackend::Init called twice without Shutdown");
    RS_CORE_TRACE("RaylibImGuiBackend: initializing rlImGui");
    rlImGuiSetup(true);
    m_Initialized = true;
    RS_CORE_DEBUG("RaylibImGuiBackend: initialized");
}

void RaylibImGuiBackend::Shutdown()
{
    RS_PROFILE_FUNCTION();
    if (!m_Initialized)
    {
        RS_CORE_WARN("RaylibImGuiBackend::Shutdown called but backend was not initialized");
        return;
    }

    RS_CORE_TRACE("RaylibImGuiBackend: shutting down");
    rlImGuiShutdown();
    m_Initialized = false;
    RS_CORE_DEBUG("RaylibImGuiBackend: shut down");
}

void RaylibImGuiBackend::BeginFrame()
{
    RS_PROFILE_FUNCTION();
    if (!m_Initialized)
    {
        RS_CORE_WARN("RaylibImGuiBackend::BeginFrame called but backend is not initialized");
        return;
    }

    rlImGuiBegin();
}

void RaylibImGuiBackend::EndFrame()
{
    RS_PROFILE_FUNCTION();
    if (!m_Initialized)
    {
        RS_CORE_WARN("RaylibImGuiBackend::EndFrame called but backend is not initialized");
        return;
    }

    rlImGuiEnd();
}

} // namespace RS::Backend
