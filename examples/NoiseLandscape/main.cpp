//==============================================================================
//  Noise Landscape
//  Procedurally generated scrolling terrain using Math::Random noise functions.
//
//  Controls:
//    [1]     Perlin noise
//    [2]     Simplex noise
//    [3]     Cellular noise
//    [4]     Value noise
//    [5]     Fractal Brownian Motion
//    [SPACE] Pause / Resume auto-scroll
//    [R]     Re-seed (randomise terrain + star field)
//    [+]     Increase scroll speed
//    [-]     Decrease scroll speed
//==============================================================================

#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"
#include "SkyLayer.hpp"
#include "TerrainLayer.hpp"

#include <optional>
#include <string>

using namespace RS;

//==============================================================================
// NoiseLandscape
//==============================================================================

class NoiseLandscape : public Scene {
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
        std::string modeLine  = "[1-5] Mode: " + std::string(NoiseName(terrain_->GetMode()));
        std::string pauseLine = "[SPACE] " + std::string(autoScroll_ ? "Scrolling" : "Paused  ");
        std::string speedLine = "[Up/Down] Speed: " + std::to_string(static_cast<int>(scrollSpeed_)) + " px/s";
        std::string reseedLine= "[R] Reseed";
        std::string fpsLine   = "FPS: " + std::to_string(static_cast<int>(Time::GetSmoothedFPS()));

        Text::DrawText(modeLine,   16.0f, 16.0f, 22, Colors::White);
        Text::DrawText(pauseLine,  16.0f, 44.0f, 18, Colors::LightGray);
        Text::DrawText(speedLine,  16.0f, 68.0f, 18, Colors::LightGray);
        Text::DrawText(reseedLine, 16.0f, 92.0f, 18, Colors::LightGray);
        Text::DrawText(fpsLine, static_cast<float>(GetContext().MainWindow->GetWidth()) - 16.0f, 16.0f, 20,
                       Colors::LightGray, OriginMode::TopRight);
    }

#pragma endregion

public:

//==============================================================================
// Lifecycle
//==============================================================================

    void OnStart() override
    {
        GetContext().MainWindow->SetSize(1200, 700);
        GetContext().MainWindow->SetTitle("Raysim - Noise Landscape");
        FontManager::LoadFont("assets/fonts/OpenSans-Regular.ttf");
        Time::SetTargetFPS(60);
        float w = static_cast<float>(GetContext().MainWindow->GetWidth());
        float h = static_cast<float>(GetContext().MainWindow->GetHeight());
        skyLayer_.emplace(w, h * SKY_HEIGHT_RATIO, STAR_COUNT, SKY_GRAD_STEPS);
        terrain_.emplace(w, h);
    }

    void OnUpdate(float dt) override
    {
        //--- Mode select ------------------------------------------------
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::One))   terrain_->SetMode(NoiseMode::Perlin);
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::Two))   terrain_->SetMode(NoiseMode::Simplex);
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::Three)) terrain_->SetMode(NoiseMode::Cellular);
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::Four))  terrain_->SetMode(NoiseMode::Value);
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::Five))  terrain_->SetMode(NoiseMode::FBM);

        //--- Scroll control ---------------------------------------------
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::Space)) autoScroll_ = !autoScroll_;

        if (GetContext().InputSystem->IsKeyDown(KeyCode::Up))
            scrollSpeed_ = Math::Min(scrollSpeed_ + 80.0f * dt, 600.0f);
        if (GetContext().InputSystem->IsKeyDown(KeyCode::Down))
            scrollSpeed_ = Math::Max(scrollSpeed_ - 80.0f * dt, 5.0f);

        //--- Reseed - regenerate both sky and terrain -----------------------
        if (GetContext().InputSystem->IsKeyPressed(KeyCode::R)) {
            Math::Random::SeedRandom();
            skyLayer_->Reseed();
            terrain_->Reseed();
        }

        //--- Advance scroll ---------------------------------------------
        if (autoScroll_) scrollX_ += scrollSpeed_ * dt;
        terrain_->SetScrollX(scrollX_);
    }

    void OnDraw(float /*alpha*/) override
    {
        float skyH = static_cast<float>(GetContext().MainWindow->GetHeight()) * SKY_HEIGHT_RATIO;
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
    config.Window.Title  = "Raysim - Noise Landscape Demo";
    config.Window.Width  = 1000;
    config.Window.Height = 600;

    auto* app = new RS::Application(config);
    app->AddScene(RS::CreateScope<NoiseLandscape>());
    return app;
}
