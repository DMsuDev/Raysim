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
#include "SkyLayer.hpp"
#include "TerrainLayer.hpp"

#include <optional>
#include <string>

using namespace RS;

//==============================================================================
// NoiseLandscape
//==============================================================================

class NoiseLandscape : public Application {
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
        Text::DrawText(fpsLine, static_cast<float>(GetWidth()) - 16.0f, 16.0f, 20,
                       Colors::LightGray, OriginMode::TopRight);
    }

#pragma endregion

public:

//==============================================================================
// Lifecycle
//==============================================================================

    void Setup() override
    {
        SetSize(1200, 700);
        SetTitle("Raysim - Noise Landscape");
        SetDefaultFont("assets/fonts/OpenSans-Regular.ttf");
        Time::SetTargetFPS(60);
        float w = static_cast<float>(GetWidth());
        float h = static_cast<float>(GetHeight());
        skyLayer_.emplace(w, h * SKY_HEIGHT_RATIO, STAR_COUNT, SKY_GRAD_STEPS);
        terrain_.emplace(w, h);
    }

    void Update(float dt) override
    {
        //--- Mode select ------------------------------------------------
        if (Input->IsKeyPressed(KeyCode::One))   terrain_->SetMode(NoiseMode::Perlin);
        if (Input->IsKeyPressed(KeyCode::Two))   terrain_->SetMode(NoiseMode::Simplex);
        if (Input->IsKeyPressed(KeyCode::Three)) terrain_->SetMode(NoiseMode::Cellular);
        if (Input->IsKeyPressed(KeyCode::Four))  terrain_->SetMode(NoiseMode::Value);
        if (Input->IsKeyPressed(KeyCode::Five))  terrain_->SetMode(NoiseMode::FBM);

        //--- Scroll control ---------------------------------------------
        if (Input->IsKeyPressed(KeyCode::Space)) autoScroll_ = !autoScroll_;

        if (Input->IsKeyDown(KeyCode::Up))
            scrollSpeed_ = Math::Min(scrollSpeed_ + 80.0f * dt, 600.0f);
        if (Input->IsKeyDown(KeyCode::Down))
            scrollSpeed_ = Math::Max(scrollSpeed_ - 80.0f * dt, 5.0f);

        //--- Reseed - regenerate both sky and terrain -----------------------
        if (Input->IsKeyPressed(KeyCode::R)) {
            Math::Random::SeedRandom();
            skyLayer_->Reseed();
            terrain_->Reseed();
        }

        //--- Advance scroll ---------------------------------------------
        if (autoScroll_) scrollX_ += scrollSpeed_ * dt;
        terrain_->SetScrollX(scrollX_);
    }

    void Draw(float /*alpha*/) override
    {
        float skyH = static_cast<float>(GetHeight()) * SKY_HEIGHT_RATIO;
        skyLayer_->SetSkyHeight(skyH);
        skyLayer_->DrawSky();               // gradient background
        skyLayer_->DrawStars();             // twinkling star field
        skyLayer_->DrawMoon();              // moon + glow halos
        terrain_->DrawFarRidgeline(skyH);   // parallax far ridgeline
        terrain_->DrawTerrain(skyH);        // main scrolling terrain
        DrawHUD();
    }
};

int main()
{
    NoiseLandscape sim;
    sim.Run();
    return 0;
}
