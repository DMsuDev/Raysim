#pragma once

#include "Raysim/Raysim.hpp"
#include "SceneNavLayer.hpp"

#include <vector>

using namespace RS;

// ============================================================================
// StarFieldControlLayer
// In-scene ImGui panel: warp speed + rainbow toggle.
// ============================================================================

class StarFieldControlLayer : public Layer
{
public:
    StarFieldControlLayer(float& speed, bool& rainbow)
        : Layer("StarFieldCtrl"), m_Speed(speed), m_Rainbow(rainbow) {}

    void OnUIRender() override;

private:
    float& m_Speed;
    bool&  m_Rainbow;
};

// ============================================================================
// StarField Scene 1
// 300 stars in 3-D perspective rushing toward the viewer.
// ============================================================================

class StarField : public Scene
{
    RS_SCENE(StarField)

    struct Star { float x, y, z, hue; };

    static constexpr int   NUM_STARS  = 300;
    static constexpr float BASE_SPEED = 0.35f;

    std::vector<Star> m_Stars;
    float m_Speed   = 1.0f;
    bool  m_Rainbow = false;

    void Reset(Star& s, bool spread = true);

public:
    void OnStart()                   override;
    void OnUpdate(float dt)          override;
    void OnDraw(float /*alpha*/)     override;
};
