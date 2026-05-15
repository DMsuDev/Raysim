#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include "example_ball.hpp"
#include "example_helpers.hpp"

#include <vector>
#include <array>

using namespace RS;

//==============================================================================
// Simulation
//==============================================================================

class BouncingBalls : public Scene {
    RS_SCENE(BouncingBalls)

private:
    static constexpr float GRAVITY_Y        = 400.0f;
    static constexpr float MOUSE_FORCE      = 2500.0f;
    static constexpr float INFLUENCE_RADIUS = 150.0f;
    static constexpr float MIN_INFLUENCE    = 10.0f;
    static constexpr int   INITIAL_BALLS    = 10;
    static constexpr float BALL_SIZE_MIN    = 16.0f;
    static constexpr float BALL_SIZE_MAX    = 28.0f;

    // Background configuration using helper defaults
    BouncingBallsExample::BackgroundConfig bgConfig;
    std::vector<Ball> balls_;
    static constexpr std::array<Color, 6> BALL_COLORS = {{
        {255, 102, 102}, {102, 204, 255}, {153, 255, 153},
        {255, 204, 102}, {204, 153, 255}, {255, 153, 204},
    }};

    float hueOffset_        = 0.0f;
    bool useGravity_        = false;
    bool showVelocityLines_ = true;
    bool showStats_         = true;
    int bgTheme_            = 0;

    Math::Vec2f mousePos_    = {};
    bool rightMouseDown_  = false;
    bool middleMouseDown_ = false;

    // Spatial grid for O(n) broad-phase collision detection.
    // Cell size must be >= max ball diameter so every colliding pair shares a cell or adjacent cells.
    static constexpr int GRID_CELL = static_cast<int>(BALL_SIZE_MAX) * 2 + 4; // 60 px
    std::vector<std::vector<int>> grid_;
    int gridCols_ = 0;
    int gridRows_ = 0;

//==============================================================================
// Physics helpers
//==============================================================================

    void ApplyMouseForce(float dt, bool attract) {
        const float radiusSq     = INFLUENCE_RADIUS * INFLUENCE_RADIUS;
        const float minInfluenceSq = MIN_INFLUENCE * MIN_INFLUENCE;
        const float sign         = attract ? 1.0f : -1.0f;
        for (auto& ball : balls_) {
            Math::Vec2f delta = mousePos_ - ball.position;
            float distSq = delta.LengthSquared();
            // Early-out: skip sqrt for balls outside the influence radius
            if (distSq >= radiusSq || distSq < minInfluenceSq) continue;
            float dist    = delta.Length();
            float t       = dist / INFLUENCE_RADIUS;
            ball.velocity += (delta / dist) * (MOUSE_FORCE * (1.0f - t * t) * sign * dt);
            ball.velocity.ClampLength(Ball::MAX_VELOCITY);
            ball.isMoving = true;
        }
    }

    void ResolveBallCollisions() {
        // --- Build spatial grid ---
        const int cols = window().GetWidth()  / GRID_CELL + 2;
        const int rows = window().GetHeight() / GRID_CELL + 2;

        if (cols != gridCols_ || rows != gridRows_) {
            gridCols_ = cols;
            gridRows_ = rows;
            grid_.assign(cols * rows, {});
        } else {
            for (auto& cell : grid_) cell.clear();
        }

        for (int i = 0; i < static_cast<int>(balls_.size()); ++i) {
            int cx = Math::Clamp(static_cast<int>(balls_[i].position.x) / GRID_CELL, 0, cols - 1);
            int cy = Math::Clamp(static_cast<int>(balls_[i].position.y) / GRID_CELL, 0, rows - 1);
            grid_[cy * cols + cx].push_back(i);
        }

        // Resolve collision between two ball indices
        auto resolve = [&](int i, int j) {
            Ball& a = balls_[i];
            Ball& b = balls_[j];
            Math::Vec2f delta = b.position - a.position;
            float minDist = a.radius + b.radius;
            // Cheap squared check before computing sqrt
            if (delta.LengthSquared() >= minDist * minDist) return;
            float dist = delta.Length();
            if (dist <= 0.0f) return;
            Math::Vec2f normal = delta / dist;
            float overlap = (minDist - dist) * 0.5f;
            a.position -= normal * overlap;
            b.position += normal * overlap;
            std::swap(a.velocity, b.velocity);
            a.isMoving = b.isMoving = true;
        };

        // --- Check each cell vs itself and 4 forward neighbors (right, down-left, down, down-right)
        // This visits every unique pair exactly once.
        static constexpr int dxs[] = {1, -1, 0, 1};
        static constexpr int dys[] = {0,  1, 1, 1};

        for (int cy = 0; cy < rows; ++cy) {
            for (int cx = 0; cx < cols; ++cx) {
                const auto& cell = grid_[cy * cols + cx];
                // Pairs within the same cell
                for (size_t i = 0; i < cell.size(); ++i)
                    for (size_t j = i + 1; j < cell.size(); ++j)
                        resolve(cell[i], cell[j]);
                // Pairs across forward-neighbor cells
                for (int n = 0; n < 4; ++n) {
                    int nx = cx + dxs[n], ny = cy + dys[n];
                    if (nx < 0 || nx >= cols || ny >= rows) continue;
                    for (int a : cell)
                        for (int b : grid_[ny * cols + nx])
                            resolve(a, b);
                }
            }
        }
    }

//==============================================================================
// Drawing helpers
//==============================================================================

    void DrawBackground() {
        BouncingBallsExample::DrawBackground(window().GetWidth(), window().GetHeight(),
                                             bgTheme_, Time::GetTime(), bgConfig);
    }

    void DrawMouseCursors() {
        float t = Time::GetTime();
        float pulseR = INFLUENCE_RADIUS + Math::PingPong(t * 4.0f, 8.0f) - 4.0f;

        Shapes::DrawCircle(mousePos_.x, mousePos_.y, 8.0f, Colors::RayCyan, OriginMode::Center);
        if (rightMouseDown_)
            Shapes::DrawCircleOutline(mousePos_.x, mousePos_.y, pulseR, Colors::RayRed);
        if (middleMouseDown_)
            Shapes::DrawCircleOutline(mousePos_.x, mousePos_.y, pulseR, {100, 255, 150, 150});
    }

    void DrawStats() {
        float t = Time::GetTime();
        float w = static_cast<float>(window().GetWidth());
        float h = static_cast<float>(window().GetHeight());

        BouncingBallsExample::DrawChip(18, 20,
                 fmt::format("Balls: {}", static_cast<int>(balls_.size())),
                 {70, 130, 180, 180}, t);
        BouncingBallsExample::DrawChip(w - 180.0f, 20,
                 fmt::format("FPS: {}", static_cast<int>(Time::GetSmoothedFPS())),
                 {140, 90, 200, 180}, t, 4.0f);
        BouncingBallsExample::DrawChip(18, h - 60.0f,
                 fmt::format("Gravity: {}", useGravity_ ? "ON" : "OFF"),
                 {200, 120, 80, 180}, t, 3.0f);
        BouncingBallsExample::DrawChip(w - 220.0f, h - 60.0f,
                 fmt::format("Velocity: {}", showVelocityLines_ ? "ON" : "OFF"),
                 {100, 200, 150, 180}, t, 3.0f);
        BouncingBallsExample::DrawChip(w * 0.5f - 80.0f, h - 60.0f,
                 fmt::format("Theme: {}", BouncingBallsExample::BG_THEMES[bgTheme_].name),
                 {80, 150, 220, 180}, t, 3.5f);

        Text::RenderText("[LMB] Add  [RMB] Repel  [MMB] Attract  [G] Gravity  [V] Velocity  [S] Stats  [C] Color",
                       20, h - 100.0f, 20, {200, 200, 220}, OriginMode::TopLeft);
    }

//==============================================================================
// Spawn helpers
//==============================================================================

    void SpawnRandomBalls(int count) {
        balls_.reserve(balls_.size() + count);
        for (int i = 0; i < count; ++i) {
            Math::Vec2f pos{
                Math::Random::Range(100.0f, window().GetWidth() - 100.0f),
                Math::Random::Range(50.0f, window().GetHeight() - 50.0f)
            };
            Math::Vec2f vel{
                Math::Random::Range(-150.0f, 150.0f),
                Math::Random::Range(-50.0f, 50.0f)
            };
            balls_.emplace_back(pos, vel,
                Math::Random::Range(BALL_SIZE_MIN, BALL_SIZE_MAX),
                Math::Random::Choice(BALL_COLORS));
        }
    }

    void SpawnAtMouse() {
        hueOffset_ = Math::Repeat(hueOffset_ + 25.0f, 360.0f);
        float angle = Math::Random::Range(0.0f, Math::TWO_PI);
        float speed = Math::Random::Range(100.0f, 300.0f);
        balls_.emplace_back(mousePos_,
            Math::Vec2f{Math::Cos(angle) * speed, Math::Sin(angle) * speed},
            Math::Random::Range(BALL_SIZE_MIN, BALL_SIZE_MAX),
            Color::FromHSV(hueOffset_, 0.85f, 1.0f));
    }

public:
//==============================================================================
// Application lifecycle
//==============================================================================

    void OnStart() override {
        FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);
        SpawnRandomBalls(INITIAL_BALLS);
    }

    void OnUpdate(float dt) override {
        mousePos_ = input().GetMousePosition();
        rightMouseDown_ = input().IsMouseButtonDown(MouseButton::Right);
        middleMouseDown_ = input().IsMouseButtonDown(MouseButton::Middle);

        if (input().IsKeyPressed(KeyCode::G)) useGravity_ = !useGravity_;
        if (input().IsKeyPressed(KeyCode::V)) showVelocityLines_ = !showVelocityLines_;
        if (input().IsKeyPressed(KeyCode::S)) showStats_ = !showStats_;
        if (input().IsKeyPressed(KeyCode::C)) bgTheme_ = (bgTheme_ + 1) % BouncingBallsExample::NUM_BG_THEMES;
        if (input().IsMouseButtonPressed(MouseButton::Left)) SpawnAtMouse();
    }

    void OnFixedUpdate(float dt) override {
        if (rightMouseDown_) ApplyMouseForce(dt, false);
        if (middleMouseDown_) ApplyMouseForce(dt, true);

        for (auto& ball : balls_) {
            if (!ball.isMoving) continue;
            if (useGravity_) ball.velocity.y += GRAVITY_Y * dt;
            ball.position += ball.velocity * dt;
            ball.HandleBoundaryCollision(window().GetWidth(), window().GetHeight());
        }

        ResolveBallCollisions();
    }

    void OnDraw(float alpha) override {
        renderer().ClearScreen(Color{18, 24, 38});
        DrawBackground();
        for (const auto& ball : balls_) {
            ball.Draw(showVelocityLines_);
        }
        DrawMouseCursors();
        if (showStats_) DrawStats();
    }
};

//==============================================================================
// Entry point
//==============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Raysim - Bouncing Balls Demo";
    config.Window.Width  = 1000;
    config.Window.Height = 720;
    config.VSync = true;
    config.TargetFPS = 60;

    auto* app = new RS::Application(config);
    app->RegisterScene<BouncingBalls>();
    app->ChangeScene<BouncingBalls>();
    return app;
}
