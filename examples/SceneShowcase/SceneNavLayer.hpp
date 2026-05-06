#pragma once

#include "Raysim/Raysim.hpp"

#include <functional>

using namespace RS;

// ============================================================================
// SceneNavLayer
// Floating top-center overlay shared by all scenes.
// Displays the current scene name and << / >> navigation buttons.
// ============================================================================

class SceneNavLayer : public Layer
{
public:
    SceneNavLayer(const char* sceneName, int index, int total,
                  std::function<void()> onNext,
                  std::function<void()> onPrev)
        : Layer("SceneNavLayer")
        , m_Name(sceneName), m_Index(index), m_Total(total)
        , m_OnNext(std::move(onNext)), m_OnPrev(std::move(onPrev))
    {}

    void OnUIRender() override
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 10.0f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.72f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##scenenav", nullptr, flags);

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.15f, 0.15f, 0.38f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.30f, 0.65f, 1.0f));

        if (ImGui::Button(" << ")) m_OnPrev();
        ImGui::SameLine();
        ImGui::Text("  %s  [%d / %d]  ", m_Name, m_Index, m_Total);
        ImGui::SameLine();
        if (ImGui::Button(" >> ")) m_OnNext();

        ImGui::PopStyleColor(2);
        ImGui::End();
    }

private:
    const char* m_Name;
    int m_Index, m_Total;
    std::function<void()> m_OnNext;
    std::function<void()> m_OnPrev;
};
