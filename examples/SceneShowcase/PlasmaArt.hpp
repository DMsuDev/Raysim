#pragma once

#include "Raysim/Raysim.hpp"
#include "SceneNavLayer.hpp"

using namespace RS;

// ============================================================================
// PlasmaControlLayer
// In-scene ImGui panel: frequency X/Y + animation speed.
// ============================================================================

class PlasmaControlLayer : public Layer
{
public:
    PlasmaControlLayer(float& fx, float& fy, float& speed)
        : Layer("PlasmaCtrl"), m_FX(fx), m_FY(fy), m_Speed(speed) {}

    void OnUIRender() override;

private:
    float& m_FX;
    float& m_FY;
    float& m_Speed;
};

// ============================================================================
// PlasmaArt Scene 2
// CPU plasma: 50×35 color tiles driven by 4 layered sine waves.
// ============================================================================

class PlasmaArt : public Scene
{
    RS_SCENE(PlasmaArt)

    static constexpr int COLS = 50;
    static constexpr int ROWS = 35;

    float m_Time  = 0.0f;
    float m_FX    = 3.0f;
    float m_FY    = 2.5f;
    float m_Speed = 1.0f;

public:
    void OnStart()               override;
    void OnUpdate(float dt)      override;
    void OnDraw(float /*alpha*/) override;
};
