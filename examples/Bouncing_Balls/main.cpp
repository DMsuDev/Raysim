// ─────────────────────────────────────────────────────────────
//  RaySim - Bouncing Balls with Advanced Physics
//  Demonstrates: Physics, mouse interaction, gravity toggling,
//                velocity visualization, glow effects, HSV colors
// ─────────────────────────────────────────────────────────────
#include "Raysim/Raysim.hpp"

#include <cmath>
#include <vector>

using namespace RS;

// ── Ball Structure ───────────────────────────────────────
struct Ball {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    bool isMoving = true;

    Ball(float x, float y, float vx, float vy, float r, Color c)
        : position{x, y}, velocity{vx, vy}, radius{r}, color{c} {}
};

// ── Physics Simulation ───────────────────────────────────
class BouncingBallsPP : public Canvas {
private:
    std::vector<Ball> balls_;
    Vector2 gravity_{0.0f, 400.0f};
    bool useGravity_ = false;
    bool showVelocityLines_ = true;
    float hueOffset_ = 0.0f;

    std::vector<Color> colors = {
        Color{255, 100, 150},   // Rosa
        Color{100, 200, 255},   // Azul claro
        Color{150, 255, 100},   // Verde claro
        Color{255, 200, 100},   // Naranja
        Color{200, 100, 255},   // Púrpura
        Color{255, 255, 100},   // Amarillo
    };

    // HSV to RGB conversion for smooth color gradients
    static Color HSVtoRGB(float h, float s, float v, unsigned char alpha = 255) {
        h = std::fmod(h, 360.0f);
        if (h < 0) h += 360.0f;

        float c = v * s;
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r, g, b;
        if (h < 60) { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        return Color{
            (unsigned char)((r + m) * 255),
            (unsigned char)((g + m) * 255),
            (unsigned char)((b + m) * 255),
            alpha
        };
    }

public:
    void Setup() override {
        SetSize(1000, 700);
        SetTitle("RaySim - Bouncing Balls Physics");
        SetDefaultFont("resources/fonts/opensans/OpenSans-Regular.ttf", 24);
        SetTargetFPS(60);

        // Create initial balls
        SpawnRandomBalls(8);
    }

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

    void SpawnAtMouse(Vector2 mousePos) {
        float angle = Random::Range(0.0f, 2.0f * 3.14159265f);
        float speed = Random::Range(100.0f, 300.0f);
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;
        float r = Random::Range(12.0f, 28.0f);

        Color col = HSVtoRGB(hueOffset_, 0.85f, 1.0f);
        hueOffset_ += 25.0f;
        if (hueOffset_ >= 360.0f) hueOffset_ -= 360.0f;

        balls_.emplace_back(mousePos.x, mousePos.y, vx, vy, r, col);
    }

    void UpdateBalls(float deltaTime) {
        for (auto& ball : balls_) {
            if (!ball.isMoving) continue;

            // Apply gravity
            if (useGravity_) {
                ball.velocity.x += gravity_.x * deltaTime;
                ball.velocity.y += gravity_.y * deltaTime;
            }

            // Update position
            ball.position.x += ball.velocity.x * deltaTime;
            ball.position.y += ball.velocity.y * deltaTime;

            // Boundary collision - horizontal
            if (ball.position.x - ball.radius < 0) {
                ball.position.x = ball.radius;
                ball.velocity.x *= -0.85f; // Damping
            }
            if (ball.position.x + ball.radius > GetWidth()) {
                ball.position.x = GetWidth() - ball.radius;
                ball.velocity.x *= -0.85f;
            }

            // Boundary collision - vertical
            if (ball.position.y - ball.radius < 0) {
                ball.position.y = ball.radius;
                ball.velocity.y *= -0.85f;
            }
            if (ball.position.y + ball.radius > GetHeight()) {
                ball.position.y = GetHeight() - ball.radius;
                ball.velocity.y *= -0.85f;

                // Stop if very slow
                float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
                if (speed < 15.0f) {
                    ball.isMoving = false;
                    ball.velocity = {0, 0};
                }
            }
        }

        // Ball-to-ball collision
        for (size_t i = 0; i < balls_.size(); ++i) {
            for (size_t j = i + 1; j < balls_.size(); ++j) {
                float dx = balls_[j].position.x - balls_[i].position.x;
                float dy = balls_[j].position.y - balls_[i].position.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                float minDist = balls_[i].radius + balls_[j].radius;

                if (distance < minDist && distance > 0.0f) {
                    // Normalize
                    dx /= distance;
                    dy /= distance;

                    // Separate balls
                    float overlap = (minDist - distance) / 2.0f;
                    balls_[i].position.x -= dx * overlap;
                    balls_[i].position.y -= dy * overlap;
                    balls_[j].position.x += dx * overlap;
                    balls_[j].position.y += dy * overlap;

                    // Swap velocities (simplified elastic collision)
                    Vector2 tempVel = balls_[i].velocity;
                    balls_[i].velocity = balls_[j].velocity;
                    balls_[j].velocity = tempVel;

                    balls_[i].isMoving = true;
                    balls_[j].isMoving = true;
                }
            }
        }
    }

    void Draw() override {
        Background(Color{10, 15, 30});

        InputManager& input = GetInputManager();
        TimeManager& time = GetTimeManager();
        Vector2 mousePos = input.GetMousePosition();
        float deltaTime = time.GetDeltaTime();

        // Update physics
        UpdateBalls(deltaTime);

        // Toggle gravity with G
        if (input.IsKeyPressed(KeyCode::G)) {
            useGravity_ = !useGravity_;
        }

        // Toggle velocity visualization with V
        if (input.IsKeyPressed(KeyCode::V)) {
            showVelocityLines_ = !showVelocityLines_;
        }

        // Add ball on left click
        if (input.IsMouseButtonPressed(MouseButton::Left)) {
            SpawnAtMouse(mousePos);
        }

        // Mouse attraction/repulsion with right click
        if (input.IsMouseButtonDown(MouseButton::Right)) {
            for (auto& ball : balls_) {
                float dx = mousePos.x - ball.position.x;
                float dy = mousePos.y - ball.position.y;

                float distance = Vector2::Distance(ball.position, mousePos);

                if (distance > 1.0f) {
                    float force = 200.0f / (distance + 1.0f);
                    ball.velocity.x += (dx / distance) * force * 0.016f;
                    ball.velocity.y += (dy / distance) * force * 0.016f;

                    // Limit velocity
                    float velMag = ball.velocity.Length();
                    if (velMag > 600.0f) {
                        ball.velocity.x = (ball.velocity.x / velMag) * 600.0f;
                        ball.velocity.y = (ball.velocity.y / velMag) * 600.0f;
                    }
                    ball.isMoving = true;
                }
            }
        }

        // ── Draw balls ───────────────────────────────────
        for (auto& ball : balls_) {
            // Glow ring based on velocity
            float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
            float glowAlpha = std::min(255.0f, (speed / 10.0f) * 200.0f);

            Shapes::DrawCircleOutline(
                ball.position.x, ball.position.y,
                ball.radius * 1.3f,
                Color{ball.color.r, ball.color.g, ball.color.b, (unsigned char)glowAlpha}
            );

            // Main ball
            Shapes::DrawCircle(ball.position.x, ball.position.y, ball.radius, ball.color, OriginMode::Center);

            // Specular highlight
            Shapes::DrawCircle(
                ball.position.x - ball.radius * 0.25f,
                ball.position.y - ball.radius * 0.25f,
                ball.radius * 0.4f,
                Color{255, 255, 255, 120},
                OriginMode::Center
            );

            // Velocity line
            if (showVelocityLines_ && speed > 10.0f) {
                Vector2 velEnd{
                    ball.position.x + ball.velocity.x * 0.03f,
                    ball.position.y + ball.velocity.y * 0.03f
                };
                Shapes::DrawLine(
                    ball.position.x, ball.position.y,
                    velEnd.x, velEnd.y,
                    Color{100, 255, 100, 200}
                );
            }
        }

        // Mouse cursor indicator
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8.0f, Color::RayCyan(), OriginMode::Center);
        if (input.IsMouseButtonDown(MouseButton::Right)) {
            Shapes::DrawCircleOutline(mousePos.x, mousePos.y, 60.0f, Color{255, 100, 100, 150});
        }

        // ── HUD ──────────────────────────────────────────
        Shapes::DrawRect(10, 10, 350, 150, Color{20, 30, 60, 230}, OriginMode::TopLeft);
        Shapes::DrawRectOutline(10, 10, 350, 150, Color{100, 200, 255});

        std::string ballsText = "Balls: " + std::to_string((int)balls_.size());
        Text::DrawText(ballsText, 20, 20, 14, Color{150, 255, 100}, OriginMode::TopLeft);

        std::string gravityText = "Gravity: " + std::string(useGravity_ ? "ON (G)" : "OFF (G)");
        Text::DrawText(gravityText, 20, 40, 14, Color{255, 200, 100}, OriginMode::TopLeft);

        std::string velText = "Velocity Lines: " + std::string(showVelocityLines_ ? "ON (V)" : "OFF (V)");
        Text::DrawText(velText, 20, 60, 14, Color{100, 200, 255}, OriginMode::TopLeft);

        std::string fpsText = "FPS: " + std::to_string(time.GetCurrentFPS());
        Text::DrawText(fpsText, 20, 80, 14, Color{200, 100, 255}, OriginMode::TopLeft);

        Text::DrawText("LMB: Add Ball  |  RMB: Repel  |  G: Gravity  |  V: Velocity Lines", 10, GetHeight() - 20, 13, Color::LightGray(), OriginMode::BottomLeft);
    }
};

int main() {
    BouncingBallsPP sim;
    sim.Run();
    return 0;
}
