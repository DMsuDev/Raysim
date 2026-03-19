//==============================================================================
//  Bouncing Balls
//  Physics-based 2D simulation: spawn, attract and repel balls
//  with configurable gravity and mouse interaction.
//
//  Controls:
//    [LMB]  Spawn a ball at the cursor
//    [RMB]  Repel nearby balls
//    [MMB]  Attract nearby balls
//    [G]    Toggle gravity
//    [V]    Toggle velocity vectors
//    [S]    Toggle stats overlay
//==============================================================================

#include "Raysim/Raysim.hpp"
#include <cmath>
#include <vector>

using namespace RS;

//==============================================================================
// Ball
//==============================================================================

struct Ball {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    Color   color;
    bool    isMoving = true;

    Ball(float x, float y, float vx, float vy, float r, Color c)
        : position{x, y}, velocity{vx, vy}, radius{r}, color{c} {}
};

//==============================================================================
// Simulation
//==============================================================================

class BouncingBalls : public Application {
private:

#pragma region State

    std::vector<Ball> balls_;
    float hueOffset_ = 0.0f;

    //--- Flags -------------------------------------------------------
    bool useGravity_        = false;
    bool showVelocityLines_ = true;
    bool showStats_         = true;

    //--- Mouse state (set in Update, consumed in FixedUpdate) --------
    Vector2 mousePos_        = {};
    bool    rightMouseDown_  = false;
    bool    middleMouseDown_ = false;

    //--- Physics constants -------------------------------------------
    static constexpr float GRAVITY_Y        = 400.0f;
    static constexpr float BALL_DAMPING     = 0.85f;
    static constexpr float STOP_THRESHOLD   = 15.0f;
    static constexpr float MAX_VELOCITY     = 600.0f;
    static constexpr float MOUSE_FORCE      = 2500.0f;
    static constexpr float INFLUENCE_RADIUS = 150.0f;
    static constexpr float MIN_INFLUENCE    = 10.0f;

    //--- Color palette -----------------------------------------------
    std::vector<Color> palette_ = {
        {255, 102, 102},
        {102, 204, 255},
        {153, 255, 153},
        {255, 204, 102},
        {204, 153, 255},
        {255, 153, 204},
    };

#pragma endregion

//==============================================================================
// Physics helpers
//==============================================================================

#pragma region Physics

    void HandleBoundaryCollision(Ball& ball) {
        //--- Left / right ------------------------------------------------
        if (ball.position.x - ball.radius < 0) {
            ball.position.x  = ball.radius;
            ball.velocity.x *= -BALL_DAMPING;
        } else if (ball.position.x + ball.radius > GetWidth()) {
            ball.position.x  = GetWidth() - ball.radius;
            ball.velocity.x *= -BALL_DAMPING;
        }

        //--- Top / bottom ------------------------------------------------
        if (ball.position.y - ball.radius < 0) {
            ball.position.y  = ball.radius;
            ball.velocity.y *= -BALL_DAMPING;
        } else if (ball.position.y + ball.radius > GetHeight()) {
            ball.position.y  = GetHeight() - ball.radius;
            ball.velocity.y *= -BALL_DAMPING;

            if (ball.velocity.Length() < STOP_THRESHOLD) {
                ball.velocity = {};
                ball.isMoving = false;
            }
        }
    }

    void ApplyMouseForce(float dt, bool attract) {
        for (auto& ball : balls_) {

            Vector2 delta    = mousePos_ - ball.position;
            float   distance = delta.Length();

            if (distance < INFLUENCE_RADIUS && distance > MIN_INFLUENCE) {
                float t        = distance / INFLUENCE_RADIUS;
                float forceMag = MOUSE_FORCE * (1.0f - t * t) * (attract ? 1.0f : -1.0f);
                ball.velocity += delta.Normalized() * (forceMag * dt);
                ball.velocity.Limit(MAX_VELOCITY);
                ball.isMoving = true;
            }
        }
    }

    void ResolveBallCollisions() {
        for (size_t i = 0; i < balls_.size(); ++i) {
            for (size_t j = i + 1; j < balls_.size(); ++j) {
                Vector2 delta   = balls_[j].position - balls_[i].position;
                float   dist    = delta.Length();
                float   minDist = balls_[i].radius + balls_[j].radius;

                if (dist < minDist && dist > 0.0f) {
                    Vector2 normal  = delta / dist;
                    float   overlap = (minDist - dist) * 0.5f;

                    balls_[i].position -= normal * overlap;
                    balls_[j].position += normal * overlap;
                    std::swap(balls_[i].velocity, balls_[j].velocity);
                    balls_[i].isMoving = balls_[j].isMoving = true;
                }
            }
        }
    }

#pragma endregion

//==============================================================================
// Drawing helpers
//==============================================================================

#pragma region Drawing

    void DrawBackground() {
        Vector2 center{ GetWidth() * 0.5f, GetHeight() * 0.45f };
        float   maxR = Math::Max(GetWidth(), GetHeight()) * 0.95f;
        Color   base{18, 24, 38};

        const int STEPS = 28;
        for (int i = 0; i < STEPS; ++i) {
            float f     = static_cast<float>(i) / static_cast<float>(STEPS);
            float r     = maxR * (1.0f - f * 0.95f);
            auto  alpha = static_cast<unsigned char>(Math::Remap(f, 0.0f, 1.0f, 220.0f, 6.0f));
            Shapes::DrawCircle(center.x, center.y, r, {base.r, base.g, base.b, alpha}, OriginMode::Center);
        }
    }

    void DrawBall(const Ball& ball) {
        float speed = ball.velocity.Length();
        float t     = Time::GetTime();

        //--- Pulsing glow outline ----------------------------------------
        float pulse     = Math::PingPong(t * 2.0f + ball.position.x * 0.005f, 0.3f);
        auto  glowAlpha = (unsigned char)Math::Clamp(speed * 0.4f + pulse * 80.0f, 0.0f, 255.0f);
        Shapes::DrawCircleOutline(ball.position.x, ball.position.y,
                                  ball.radius * (1.3f + pulse * 0.15f),
                                  {ball.color.r, ball.color.g, ball.color.b, glowAlpha});

        //--- Main fill ---------------------------------------------------
        Shapes::DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color, OriginMode::Center);

        //--- Highlight reflection ----------------------------------------
        Shapes::DrawCircle(ball.position.x - ball.radius * 0.25f,
                           ball.position.y - ball.radius * 0.25f,
                           ball.radius * 0.4f,
                           {255, 255, 255, 120}, OriginMode::Center);

        //--- Velocity line -----------------------------------------------
        if (showVelocityLines_ && speed > 10.0f) {
            Shapes::DrawLine(ball.position.x, ball.position.y,
                             ball.position.x + ball.velocity.x * 0.03f,
                             ball.position.y + ball.velocity.y * 0.03f,
                             {100, 255, 150, 200});
        }
    }

    void DrawChip(float x, float y, const std::string& text, const Color& col, float t, float sway = 6.0f) {
        float bob = Math::PingPong(t * 2.0f + x * 0.01f, sway * 2.0f) - sway;
        float ox  = x;
        float oy  = y + bob;

        Shapes::DrawRect(ox + 2, oy + 2, 160, 36, {0, 0, 0, 90},              OriginMode::TopLeft);
        Shapes::DrawRect(ox,     oy,     160, 36, {col.r, col.g, col.b, 160}, OriginMode::TopLeft);
        Shapes::DrawRectOutline(ox, oy, 160, 36, {255, 255, 255, 30});
        Text::DrawText(text, ox + 12, oy + 8, 24, {245, 245, 245}, OriginMode::TopLeft);
    }

    void DrawMouseCursors() {
        float t      = Time::GetTime();
        float pulseR = INFLUENCE_RADIUS + Math::PingPong(t * 4.0f, 8.0f) - 4.0f;

        Shapes::DrawCircle(mousePos_.x, mousePos_.y, 8.0f, Colors::RayCyan, OriginMode::Center);
        if (rightMouseDown_)
            Shapes::DrawCircleOutline(mousePos_.x, mousePos_.y, pulseR, Colors::RayRed);
        if (middleMouseDown_)
            Shapes::DrawCircleOutline(mousePos_.x, mousePos_.y, pulseR, {100, 255, 150, 150});
    }

    void DrawStats() {
        float t = Time::GetTime();

        float w = static_cast<float>(GetWidth());
        float h = static_cast<float>(GetHeight());

        DrawChip(18,         20,       "Balls: "   + std::to_string(static_cast<int>(balls_.size())),                     {70,  130, 180, 180}, t);
        DrawChip(w - 180.0f, 20,       "FPS: "     + std::to_string(static_cast<int>(Time::GetSmoothedFPS())), {140, 90,  200, 180}, t, 4.0f);
        DrawChip(18,         h - 60.0f, "Gravity: "  + std::string(useGravity_        ? "ON" : "OFF"), {200, 120, 80,  180}, t, 3.0f);
        DrawChip(w - 220.0f, h - 60.0f, "Velocity: " + std::string(showVelocityLines_ ? "ON" : "OFF"), {100, 200, 150, 180}, t, 3.0f);

        Text::DrawText("[LMB] Add  [RMB] Repel  [MMB] Attract  [G] Gravity  [V] Velocity  [S] Stats",
                       20, h - 100.0f, 20, {200, 200, 220}, OriginMode::TopLeft);
    }

#pragma endregion

//==============================================================================
// Spawn helpers
//==============================================================================

#pragma region Spawning

    void SpawnRandomBalls(int count) {
        for (int i = 0; i < count; ++i) {
            float x   = Math::Random::Range(100.0f, 900.0f);
            float y   = Math::Random::Range(50.0f,  300.0f);
            float vx  = Math::Random::Range(-150.0f, 150.0f);
            float vy  = Math::Random::Range(-50.0f,   50.0f);
            float r   = Math::Random::Range(12.0f, 28.0f);
            Color col = Math::Random::Choice(palette_);
            balls_.emplace_back(x, y, vx, vy, r, col);
        }
    }

    void SpawnAtMouse() {
        float angle = Math::Random::Range(0.0f, Math::TWO_PI);
        float speed = Math::Random::Range(100.0f, 300.0f);
        float r     = Math::Random::Range(12.0f, 28.0f);
        hueOffset_  = Math::Repeat(hueOffset_ + 25.0f, 360.0f);
        Color col   = Color::FromHSV(hueOffset_, 0.85f, 1.0f);
        balls_.emplace_back(mousePos_.x, mousePos_.y,
                            std::cos(angle) * speed,
                            std::sin(angle) * speed,
                            r, col);
    }

#pragma endregion

public:

//==============================================================================
// Application lifecycle
//==============================================================================

#pragma region Setup

    void Setup() override {
        SetSize(1000, 700);
        SetTitle("Raysim - Bouncing Balls Physics");
        SetDefaultFont("assets/fonts/OpenSans-Regular.ttf");
        SpawnRandomBalls(8);
        Time::SetTargetFPS(60);
    }

#pragma endregion

#pragma region Update and FixedUpdate

    void Update(float dt) override {
        mousePos_        = Input->GetMousePosition();
        rightMouseDown_  = Input->IsMouseButtonDown(MouseButton::Right);
        middleMouseDown_ = Input->IsMouseButtonDown(MouseButton::Middle);

        //--- Toggle flags ------------------------------------------------
        if (Input->IsKeyPressed(KeyCode::G)) useGravity_        = !useGravity_;
        if (Input->IsKeyPressed(KeyCode::V)) showVelocityLines_ = !showVelocityLines_;
        if (Input->IsKeyPressed(KeyCode::S)) showStats_         = !showStats_;

        //--- Spawn -------------------------------------------------------
        if (Input->IsMouseButtonPressed(MouseButton::Left)) SpawnAtMouse();
    }

    void FixedUpdate(float dt) override {
        //--- Mouse forces ------------------------------------------------
        if (rightMouseDown_)  ApplyMouseForce(dt, false);
        if (middleMouseDown_) ApplyMouseForce(dt, true);

        //--- Integrate velocity, apply gravity, resolve boundaries -------
        for (auto& ball : balls_) {
            if (!ball.isMoving) continue;
            if (useGravity_) ball.velocity.y += GRAVITY_Y * dt;
            ball.position += ball.velocity * dt;
            HandleBoundaryCollision(ball);
        }

        //--- Ball-to-ball collisions -------------------------------------
        ResolveBallCollisions();
    }

#pragma endregion

#pragma region Draw

    void Draw(float alpha) override {
        DrawBackground();
        for (const auto& ball : balls_) DrawBall(ball);
        DrawMouseCursors();
        if (showStats_) DrawStats();
    }

#pragma endregion

};

//==============================================================================
// Entry point
//==============================================================================

int main() {
    BouncingBalls sim;
    sim.Run();
    return 0;
}
