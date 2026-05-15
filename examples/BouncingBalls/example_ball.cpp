#include "example_ball.hpp"

#include "Raysim/Graphics.hpp"
#include "Raysim/Core/Time.hpp"

namespace RS {

// ============================================================================
// Physics and Collision
// ============================================================================

void Ball::HandleBoundaryCollision(int screenWidth, int screenHeight)
{
    // Left and right boundary collision
    if (position.x - radius < 0) {
        position.x = radius;
        velocity.x *= -DAMPING;
    } else if (position.x + radius > screenWidth) {
        position.x = screenWidth - radius;
        velocity.x *= -DAMPING;
    }

    // Top and bottom boundary collision
    if (position.y - radius < 0) {
        position.y = radius;
        velocity.y *= -DAMPING;
    } else if (position.y + radius > screenHeight) {
        position.y = screenHeight - radius;
        velocity.y *= -DAMPING;

        // Stop the ball if it's moving very slowly
        if (velocity.LengthSquared() < STOP_THRESHOLD * STOP_THRESHOLD) {
            velocity = {};
            isMoving = false;
        }
    }
}

// ============================================================================
// Rendering
// ============================================================================

void Ball::DrawVelocityArrow(float speed) const
{
    if (speed < 10.0f) return;

    // Calculate arrow length proportional to velocity
    float arrowLen = Math::Clamp(speed * ARROW_SPEED_SCALE, ARROW_MIN_LENGTH, ARROW_MAX_LENGTH);
    Math::Vec2f direction = velocity.Normalized();
    Math::Vec2f tipPos = position + direction * arrowLen;

    // Compute arrow color based on speed (cool -> warm gradient)
    float speedNormalized = Math::Clamp(speed / 400.0f, 0.0f, 1.0f);
    unsigned char r = static_cast<unsigned char>(Math::Lerp(100.0f, 255.0f, speedNormalized));
    unsigned char g = static_cast<unsigned char>(Math::Lerp(255.0f, 150.0f, speedNormalized));
    unsigned char b = static_cast<unsigned char>(Math::Lerp(150.0f, 100.0f, speedNormalized));

    unsigned char alphaBase = 220;

    // Draw arrow shaft: three segments with fading opacity toward the tip
    static constexpr int SHAFT_SEGMENTS = 3;
    for (int i = 0; i < SHAFT_SEGMENTS; ++i) {
        float t0 = static_cast<float>(i)     / SHAFT_SEGMENTS;
        float t1 = static_cast<float>(i + 1) / SHAFT_SEGMENTS;
        float fade = 1.0f - t0 * 0.5f;
        auto alpha = static_cast<unsigned char>(alphaBase * fade);
        Math::Vec2f p0 = position + direction * (arrowLen * t0);
        Math::Vec2f p1 = position + direction * (arrowLen * t1);
        Shapes::DrawLine(p0.x, p0.y, p1.x, p1.y, {r, g, b, alpha});
    }

    // Draw arrowhead (triangle)
    // Perpendicular vectors for left and right wings
    float arrowHeadLen = arrowLen * 0.25f;
    float arrowHeadWidth = arrowHeadLen * 0.6f;

    // Calculate triangle vertices
    Math::Vec2f leftWing = tipPos + Math::Vec2f{-direction.y, direction.x} * arrowHeadWidth - direction * arrowHeadLen;
    Math::Vec2f rightWing = tipPos - Math::Vec2f{-direction.y, direction.x} * arrowHeadWidth - direction * arrowHeadLen;

    // Draw triangle edges
    Shapes::DrawLine(tipPos.x, tipPos.y, leftWing.x, leftWing.y, {r, g, b, alphaBase});
    Shapes::DrawLine(tipPos.x, tipPos.y, rightWing.x, rightWing.y, {r, g, b, alphaBase});
    Shapes::DrawLine(leftWing.x, leftWing.y, rightWing.x, rightWing.y, {r, g, b, alphaBase});
}

void Ball::Draw(bool showVelocityArrow) const
{
    float speed = velocity.Length();
    float t = Time::GetTime();

    // Compute pulsing animation for glow effect
    float pulse = Math::PingPong(t * 2.0f + position.x * 0.005f, 0.3f);
    float pulsePulse = pulse * 0.15f;

    // Draw outer glow (aura) that pulses based on velocity and time
    auto glowAlpha = static_cast<unsigned char>(Math::Clamp(speed * 0.4f + pulse * 80.0f, 0.0f, 255.0f));
    Shapes::DrawCircleOutline(position.x, position.y,
                              radius * (1.3f + pulsePulse),
                              {color.r, color.g, color.b, glowAlpha});

    // Draw main ball body
    Shapes::DrawCircle(position.x, position.y, radius, color, OriginMode::Center);

    // Draw highlight reflection (small bright circle for depth)
    Shapes::DrawCircle(position.x - radius * 0.25f,
                       position.y - radius * 0.25f,
                       radius * 0.4f,
                       {255, 255, 255, 120}, OriginMode::Center);

    // Optionally draw velocity indicator arrow
    if (showVelocityArrow) {
        DrawVelocityArrow(speed);
    }
}

} // namespace RS
