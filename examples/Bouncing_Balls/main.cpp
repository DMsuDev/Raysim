#include "Raysim/Raysim.hpp"
#include <cmath>
#include <vector>

using namespace RS;

struct Ball {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    bool isMoving = true;

    Ball(float x, float y, float vx, float vy, float r, Color c)
        : position{x, y}, velocity{vx, vy}, radius{r}, color{c} {}
};

class BouncingBallsPP : public Canvas {
private:
    std::vector<Ball> balls_;

    Vector2 gravity_{0.0f, 400.0f};
    bool useGravity_ = false;
    bool showVelocityLines_ = true;
    bool showStats_ = true;
    float hueOffset_ = 0.0f;

    // ── Constants ─────────────────────────────────────────────
    static constexpr float BALL_DAMPING = 0.85f;
    static constexpr float STOP_THRESHOLD = 15.0f;
    static constexpr float MAX_VELOCITY = 600.0f;
    static constexpr float MOUSE_FORCE = 2500.0f;
    static constexpr float INFLUENCE_RADIUS = 150.0f;
    static constexpr float MIN_INFLUENCE_DIST = 10.0f;

    // ── Improved color palette matching dark background ───────
    std::vector<Color> colors = {
        Color{255, 102, 102},   // soft red
        Color{102, 204, 255},   // sky blue
        Color{153, 255, 153},   // soft green
        Color{255, 204, 102},   // warm yellow
        Color{204, 153, 255},   // pastel purple
        Color{255, 153, 204},   // pinkish
    };

    // ── Helper: Compute ball speed ───────────────────────────
    float GetBallSpeed(const Ball& ball) const { return ball.velocity.Length(); }

    // ── Collision with window boundaries ─────────────────────
    void HandleBoundaryCollision(Ball& ball) {
        // Left / Right walls
        if (ball.position.x - ball.radius < 0) {
            ball.position.x = ball.radius;
            ball.velocity.x *= -BALL_DAMPING;
        } else if (ball.position.x + ball.radius > GetWidth()) {
            ball.position.x = GetWidth() - ball.radius;
            ball.velocity.x *= -BALL_DAMPING;
        }

        // Top / Bottom walls
        if (ball.position.y - ball.radius < 0) {
            ball.position.y = ball.radius;
            ball.velocity.y *= -BALL_DAMPING;
        } else if (ball.position.y + ball.radius > GetHeight()) {
            ball.position.y = GetHeight() - ball.radius;
            ball.velocity.y *= -BALL_DAMPING;

            if (GetBallSpeed(ball) < STOP_THRESHOLD) {
                ball.isMoving = false;
                ball.velocity = {0, 0};
            }
        }
    }

    // ── Draw a single ball with glow and velocity line ───────
    void DrawBall(const Ball& ball) {
        float speed = GetBallSpeed(ball);

        // Glow effect proportional to speed
        float glowAlpha = std::min(255.0f, (speed / 10.0f) * 200.0f);
        Shapes::DrawCircleOutline(ball.position.x, ball.position.y, ball.radius * 1.3f,
                                  Color{ball.color.r, ball.color.g, ball.color.b, (unsigned char)glowAlpha});

        // Main ball
        Shapes::DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color, OriginMode::Center);

        // Highlight reflection
        Shapes::DrawCircle(ball.position.x - ball.radius * 0.25f,
                           ball.position.y - ball.radius * 0.25f,
                           ball.radius * 0.4f,
                           Color{255, 255, 255, 120}, OriginMode::Center);

        // Velocity line
        if (showVelocityLines_ && speed > 10.0f) {
            Shapes::DrawLine(ball.position.x, ball.position.y,
                             ball.position.x + ball.velocity.x * 0.03f,
                             ball.position.y + ball.velocity.y * 0.03f,
                             Color{100, 255, 150, 200});
        }
    }

    // ── Apply mouse forces (attract or repel) ────────────────
    void ApplyMouseForce(Vector2 mousePos, float deltaTime, bool attract) {
        for (auto& ball : balls_) {
            float dx = mousePos.x - ball.position.x;
            float dy = mousePos.y - ball.position.y;
            float distance = std::sqrt(dx*dx + dy*dy);

            if (distance < INFLUENCE_RADIUS && distance > MIN_INFLUENCE_DIST) {
                float nx = dx / distance;
                float ny = dy / distance;
                float normalizedDist = distance / INFLUENCE_RADIUS;
                float forceMag = MOUSE_FORCE * (1.0f - normalizedDist * normalizedDist);
                if (!attract) forceMag *= -1.0f;

                ball.velocity.x += nx * forceMag * deltaTime;
                ball.velocity.y += ny * forceMag * deltaTime;

                // Limit max velocity
                float velMag = ball.velocity.Length();
                if (velMag > MAX_VELOCITY) {
                    ball.velocity.x = (ball.velocity.x / velMag) * MAX_VELOCITY;
                    ball.velocity.y = (ball.velocity.y / velMag) * MAX_VELOCITY;
                }

                ball.isMoving = true;
            }
        }
    }

    // ── Radial gradient background ───────────────────────────
    void DrawBackground(float /*t*/) {
        Vector2 center = { GetWidth() * 0.5f, GetHeight() * 0.45f };
        float maxR = std::max(GetWidth(), GetHeight()) * 0.95f;
        Color base{18, 24, 38}; // dark background

        const int STEPS = 28;
        for (int i = 0; i < STEPS; ++i) {
            float f = (float)i / (float)STEPS;
            float r = maxR * (1.0f - f * 0.95f);
            unsigned char alpha = (unsigned char)std::max(6.0f, 220.0f * (1.0f - f));
            Shapes::DrawCircle(center.x, center.y, r, Color{base.r, base.g, base.b, alpha}, OriginMode::Center);
        }
    }

    // ── Floating UI chip ─────────────────────────────────────
    void DrawChip(float x, float y, const std::string& text, const Color& col, float t, float sway = 6.0f) {
        float bob = std::sin(t * 2.0f + x * 0.01f) * sway;
        float ox = x;
        float oy = y + bob;

        Shapes::DrawRect(ox + 2, oy + 2, 160, 36, Color{0,0,0,90}, OriginMode::TopLeft);
        Shapes::DrawRect(ox, oy, 160, 36, Color{col.r, col.g, col.b, 160}, OriginMode::TopLeft);
        Shapes::DrawRectOutline(ox, oy, 160, 36, Color{255,255,255,30});
        Text::DrawText(text, (int)(ox + 12), (int)(oy + 8), 24, Color{245,245,245}, OriginMode::TopLeft);
    }

    // ── Spawn a set of random balls ──────────────────────────
    void SpawnRandomBalls(int count) {
        for (int i = 0; i < count; ++i) {
            float x = Random::Range(100.0f, 900.0f);
            float y = Random::Range(50.0f, 300.0f);
            float vx = Random::Range(-150.0f, 150.0f);
            float vy = Random::Range(-50.0f, 50.0f);
            float r = Random::Range(12.0f, 28.0f);
            Color col = colors[Random::Range(0, (int)colors.size() - 1)];
            balls_.emplace_back(x, y, vx, vy, r, col);
        }
    }

    // ── Spawn a new ball at mouse position with random velocity ─
    void SpawnAtMouse(Vector2 mousePos) {
        float angle = Random::Range(0.0f, 2.0f * 3.14159265f);
        float speed = Random::Range(100.0f, 300.0f);
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;
        float r = Random::Range(12.0f, 28.0f);
        Color col = Color::FromHSV(hueOffset_, 0.85f, 1.0f);
        hueOffset_ += 25.0f;
        if (hueOffset_ >= 360.0f) hueOffset_ -= 360.0f;
        balls_.emplace_back(mousePos.x, mousePos.y, vx, vy, r, col);
    }

    // ── Update physics for all balls ─────────────────────────
    void UpdateBalls(float deltaTime) {
        for (auto& ball : balls_) {
            if (!ball.isMoving) continue;
            if (useGravity_) ball.velocity.y += gravity_.y * deltaTime;

            ball.position.x += ball.velocity.x * deltaTime;
            ball.position.y += ball.velocity.y * deltaTime;

            HandleBoundaryCollision(ball);
        }

        // Ball-to-ball collisions
        for (size_t i = 0; i < balls_.size(); ++i) {
            for (size_t j = i + 1; j < balls_.size(); ++j) {
                float dx = balls_[j].position.x - balls_[i].position.x;
                float dy = balls_[j].position.y - balls_[i].position.y;
                float distance = std::sqrt(dx*dx + dy*dy);
                float minDist = balls_[i].radius + balls_[j].radius;

                if (distance < minDist && distance > 0.0f) {
                    dx /= distance; dy /= distance;
                    float overlap = (minDist - distance) / 2.0f;

                    balls_[i].position.x -= dx * overlap;
                    balls_[i].position.y -= dy * overlap;
                    balls_[j].position.x += dx * overlap;
                    balls_[j].position.y += dy * overlap;

                    std::swap(balls_[i].velocity, balls_[j].velocity);
                    balls_[i].isMoving = balls_[j].isMoving = true;
                }
            }
        }
    }

public:
    // ── Setup canvas and initial balls ──────────────────────
    void Setup() override {
        SetSize(1000, 700);
        SetTitle("RaySim - Bouncing Balls Physics");
        SetDefaultFont("resources/fonts/opensans/OpenSans-Regular.ttf");
        SetTargetFPS(60);
        SpawnRandomBalls(8);
    }

    // ── Update simulation (input + physics) ─────────────────
    void Update() override {
        InputManager& input = GetInputManager();
        TimeManager& time = GetTimeManager();
        float deltaTime = time.GetDeltaTime();
        Vector2 mousePos = input.GetMousePosition();

        // Toggle options
        if (input.IsKeyPressed(KeyCode::G)) useGravity_ = !useGravity_;
        if (input.IsKeyPressed(KeyCode::V)) showVelocityLines_ = !showVelocityLines_;
        if (input.IsKeyPressed(KeyCode::S)) showStats_ = !showStats_;

        // Mouse interactions
        if (input.IsMouseButtonPressed(MouseButton::Left)) SpawnAtMouse(mousePos);
        if (input.IsMouseButtonDown(MouseButton::Right)) ApplyMouseForce(mousePos, deltaTime, false);
        if (input.IsMouseButtonDown(MouseButton::Middle)) ApplyMouseForce(mousePos, deltaTime, true);

        UpdateBalls(deltaTime);
    }

    // ── Draw canvas ─────────────────────────────────────────
    void Draw() override {
        TimeManager& time = GetTimeManager();
        float t = time.GetElapsedTime();

        DrawBackground(t);

        InputManager& input = GetInputManager();
        Vector2 mousePos = input.GetMousePosition();

        // Draw all balls
        for (const auto& ball : balls_) DrawBall(ball);

        // Draw cursor and influence circles
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8.0f, Colors::RayCyan, OriginMode::Center);
        if (input.IsMouseButtonDown(MouseButton::Right))
            Shapes::DrawCircleOutline(mousePos.x, mousePos.y, INFLUENCE_RADIUS, Colors::RayRed);
        if (input.IsMouseButtonDown(MouseButton::Middle))
            Shapes::DrawCircleOutline(mousePos.x, mousePos.y, INFLUENCE_RADIUS, Color{100,255,150,150});

        // Draw floating UI chips
        if (showStats_) {
            DrawChip(18, 20, "Balls: " + std::to_string((int)balls_.size()), Color{70,130,180,180}, t);
            DrawChip(GetWidth() - 180, 20, "FPS: " + std::to_string(time.GetCurrentFPS()), Color{140,90,200,180}, t, 4.0f);
            DrawChip(18, GetHeight() - 60, "Gravity: " + std::string(useGravity_ ? "ON" : "OFF"), Color{200,120,80,180}, t, 3.0f);
            DrawChip(GetWidth() - 220, GetHeight() - 60, "Velocity: " + std::string(showVelocityLines_ ? "ON" : "OFF"), Color{100,200,150,180}, t, 3.0f);

            Text::DrawText("[LMB] Add  [RMB] Repel  [MMB] Attract  [G] Gravity  [V] Velocity Vectors  [S] Stats",
                           20, GetHeight() - 100, 20, Color{200,200,220}, OriginMode::TopLeft);
        }
    }
};

// ── Main entry ──────────────────────────────────────────────
int main() {
    BouncingBallsPP sim;
    sim.Run();
    return 0;
}
