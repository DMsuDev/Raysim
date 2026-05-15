#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include "SkyLayer.hpp"
#include "TerrainLayer.hpp"

#include <optional>

using namespace RS;

//==============================================================================
// NoiseLandscape
//==============================================================================

class NoiseLandscape : public Scene {
    RS_SCENE(NoiseLandscape)

private:

#pragma region State

    float     scrollX_     = 0.0f;
    float     scrollSpeed_ = 150.0f;
    bool      autoScroll_  = true;

    //--- Layers ------------------------------------------------------------
    std::optional<TerrainLayer> terrain_;
    std::optional<SkyLayer>     skyLayer_;

    //--- Layout constants ---------------------------------------------------
    static constexpr int   STAR_COUNT       = 200;
    static constexpr int   SKY_GRAD_STEPS   = 60;
    static constexpr float SKY_HEIGHT_RATIO = 0.65f; // fraction of screen height

#pragma endregion

//==============================================================================
// Drawing helpers
//==============================================================================

#pragma region Drawing

    void DrawHUD()
    {
        Text::RenderText(fmt::format("[1-5] Mode: {}", NoiseName(terrain_->GetMode())),
                         16.0f, 16.0f, 22, Colors::White);
        Text::RenderText(fmt::format("[SPACE] {}", autoScroll_ ? "Scrolling" : "Paused  "),
                         16.0f, 44.0f, 18, Colors::LightGray);
        Text::RenderText(fmt::format("[Up/Down] Speed: {} px/s", static_cast<int>(scrollSpeed_)),
                         16.0f, 68.0f, 18, Colors::LightGray);
        Text::RenderText("[R] Reseed",
                         16.0f, 92.0f, 18, Colors::LightGray);
        Text::RenderText(fmt::format("FPS: {}", static_cast<int>(Time::GetSmoothedFPS())),
                         static_cast<float>(window().GetWidth()) - 16.0f, 16.0f, 20,
                         Colors::LightGray, OriginMode::TopRight);
    }

#pragma endregion

public:

//==============================================================================
// Lifecycle
//==============================================================================

    void OnStart() override
    {
        RS_DEBUG("NoiseLandscape starting");
        FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);
        float w = static_cast<float>(window().GetWidth());
        float h = static_cast<float>(window().GetHeight());
        skyLayer_.emplace(w, h * SKY_HEIGHT_RATIO, STAR_COUNT, SKY_GRAD_STEPS);
        terrain_.emplace(w, h);
    }

    void OnUpdate(float dt) override
    {
        //--- Mode select ------------------------------------------------
        if (input().IsKeyPressed(KeyCode::D1))    terrain_->SetMode(NoiseMode::Perlin);
        if (input().IsKeyPressed(KeyCode::D2))    terrain_->SetMode(NoiseMode::Simplex);
        if (input().IsKeyPressed(KeyCode::D3))    terrain_->SetMode(NoiseMode::Cellular);
        if (input().IsKeyPressed(KeyCode::D4))    terrain_->SetMode(NoiseMode::Value);
        if (input().IsKeyPressed(KeyCode::D5))    terrain_->SetMode(NoiseMode::FBM);

        //--- Scroll control ---------------------------------------------
        if (input().IsKeyPressed(KeyCode::Space)) autoScroll_ = !autoScroll_;

        if (input().IsKeyDown(KeyCode::Up))
            scrollSpeed_ = Math::MinValue(scrollSpeed_ + 80.0f * dt, 600.0f);
        if (input().IsKeyDown(KeyCode::Down))
            scrollSpeed_ = Math::MaxValue(scrollSpeed_ - 80.0f * dt, 5.0f);

        //--- Reseed - regenerate both sky and terrain -----------------------
        if (input().IsKeyPressed(KeyCode::R)) {
            Math::Random::SeedRandom();
            skyLayer_->Reseed();
            terrain_->Reseed();
        }

        //--- Advance scroll ---------------------------------------------
        if (autoScroll_) scrollX_ += scrollSpeed_ * dt;
        terrain_->SetScrollX(scrollX_);
    }

    void OnDraw(float alpha) override
    {
        RS_ASSERT(terrain_.has_value() && skyLayer_.has_value(), "Layers not initialized: was OnStart called?");
        float skyH = static_cast<float>(window().GetHeight()) * SKY_HEIGHT_RATIO;
        skyLayer_->SetSkyHeight(skyH);
        skyLayer_->DrawSky();               // gradient background
        skyLayer_->DrawStars();             // twinkling star field
        skyLayer_->DrawMoon();              // moon + glow halos
        terrain_->DrawFarRidgeline(skyH);   // parallax far ridgeline
        terrain_->DrawTerrain(skyH);        // main scrolling terrain
        DrawHUD();
    }
};


//==============================================================================
// Entry point
//==============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Raysim - Noise Landscape";
    config.Window.Width  = 1200;
    config.Window.Height = 700;
    config.VSync         = true;
    config.TargetFPS     = 60;

    auto* app = new RS::Application(config);
    app->RegisterScene<NoiseLandscape>();
    app->ChangeScene<NoiseLandscape>();
    return app;
}
