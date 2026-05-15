#include "example_layers.hpp"
#include "Raysim/Core/Time.hpp"

using namespace RS;

// ============================================================================
// MouseInfoLayer
// ============================================================================

MouseInfoLayer::MouseInfoLayer(const Math::Vec2f& mousePos, const Math::Vec2f& smoothedPos)
    : Layer("MouseInfoLayer"), m_MousePos(mousePos), m_SmoothedPos(smoothedPos)
{
}

void MouseInfoLayer::OnUIRender()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##mouseinfo", nullptr, flags);
    ImGui::Text("Mouse     : (%d, %d)", (int)m_MousePos.x, (int)m_MousePos.y);
    ImGui::Text("Smoothed  : (%d, %d)", (int)m_SmoothedPos.x, (int)m_SmoothedPos.y);
    ImGui::End();
}

// ============================================================================
// FpsLayer
// ============================================================================

FpsLayer::FpsLayer(int winWidth)
    : Layer("FpsLayer"), m_WinWidth(winWidth)
{
}

void FpsLayer::OnUIRender()
{
    float padding = 10.0f;
    ImGui::SetNextWindowPos(ImVec2((float)m_WinWidth - padding, padding), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##fps", nullptr, flags);
    ImGui::Text("FPS: %d", (int)Time::GetSmoothedFPS());
    ImGui::End();
}

// ============================================================================
// InstructionsLayer
// ============================================================================

InstructionsLayer::InstructionsLayer(int winHeight)
    : Layer("InstructionsLayer"), m_WinHeight(winHeight)
{
}

void InstructionsLayer::OnUIRender()
{
    float padding = 10.0f;
    ImGui::SetNextWindowPos(ImVec2(padding, (float)m_WinHeight - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##instructions", nullptr, flags);
    ImGui::TextUnformatted("Move your mouse! LMB / RMB / MMB to interact");
    ImGui::End();
}

// ============================================================================
// MouseButtonsLayer
// ============================================================================

MouseButtonsLayer::MouseButtonsLayer(Math::Vec2i size, const bool& left, const bool& right, const bool& middle)
    : Layer("MouseButtonsLayer")
    , m_WinSize(size), m_Left(left), m_Right(right), m_Middle(middle)
{
}

void MouseButtonsLayer::OnUIRender()
{
    float padding = 10.0f;
    ImGui::SetNextWindowPos(
        ImVec2((float)m_WinSize.x - padding, (float)m_WinSize.y - padding),
        ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.55f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##buttons", nullptr, flags);
    ImGui::Text("LMB: %s", m_Left   ? "DOWN" : "up");
    ImGui::Text("RMB: %s", m_Right  ? "DOWN" : "up");
    ImGui::Text("MMB: %s", m_Middle ? "DOWN" : "up");
    ImGui::End();
}
