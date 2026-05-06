#pragma once

#include "Raysim/Raysim.hpp"
#include "SceneNavLayer.hpp"

using namespace RS;

// ============================================================================
// MandalaControlLayer
// In-scene ImGui panel: ring count + global rotation speed.
// ============================================================================

class MandalaControlLayer : public Layer
{
public:
    MandalaControlLayer(int& rings, float& speed)
        : Layer("MandalaCtrl"), m_Rings(rings), m_Speed(speed) {}

    void OnUIRender() override;

private:
    int&   m_Rings;
    float& m_Speed;
};

// ============================================================================
// ClockMandala Scene 3
// Concentric rings of rotating polygons with pulsing starburst core.
// ============================================================================

class ClockMandala : public Scene
{
    RS_SCENE(ClockMandala)

    float m_Time  = 0.0f;
    int   m_Rings = 5;
    float m_Speed = 1.0f;

public:
    void OnStart()               override;
    void OnUpdate(float dt)      override;
    void OnDraw(float /*alpha*/) override;
};
