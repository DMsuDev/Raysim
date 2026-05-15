#pragma once

#include "Raysim/Layers.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

using namespace RS;

class MouseInfoLayer : public Layer
{
public:
    MouseInfoLayer(const Math::Vec2f& mousePos, const Math::Vec2f& smoothedPos);

    void OnUIRender() override;

private:
    const Math::Vec2f& m_MousePos;
    const Math::Vec2f& m_SmoothedPos;
};

class FpsLayer : public Layer
{
public:
    FpsLayer(int winWidth);

    void OnUIRender() override;

private:
    int m_WinWidth;
};

class InstructionsLayer : public Layer
{
public:
    InstructionsLayer(int winHeight);

    void OnUIRender() override;

private:
    int m_WinHeight;
};

class MouseButtonsLayer : public Layer
{
public:
    MouseButtonsLayer(Math::Vec2i size, const bool& left, const bool& right, const bool& middle);

    void OnUIRender() override;

private:
    Math::Vec2i m_WinSize;
    int         m_WinHeight;
    const bool& m_Left;
    const bool& m_Right;
    const bool& m_Middle;
};
