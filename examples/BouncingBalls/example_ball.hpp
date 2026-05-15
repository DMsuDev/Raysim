#pragma once

#include "Raysim/Math/Types/Vector2.hpp"
#include "Raysim/Graphics/Color.hpp"

namespace RS {

/**
 * @class Ball
 * @brief A physics-enabled ball object with rendering capabilities.
 *
 * The Ball class encapsulates all aspects of a bouncing ball in the simulation:
 * - Physics: Position, velocity, and boundary collision handling
 * - Rendering: Visual display with glow effects and optional velocity indicators
 * - Configuration: Customizable physics constants and visual parameters
 *
 * @note The ball uses simple Euler integration for physics and damping-based collision response.
 */
class Ball
{
public:
    // ========================================================================
    // Configuration Constants
    // ========================================================================

    /// Velocity damping factor applied on collision (0.0 = stop, 1.0 = no damping)
    static constexpr float DAMPING              = 0.85f;

    /// Velocity threshold below which the ball is considered stopped
    static constexpr float STOP_THRESHOLD       = 15.0f;

    /// Maximum allowed velocity magnitude (pixels per second)
    static constexpr float MAX_VELOCITY         = 600.0f;

    /// Velocity scale factor for arrow length calculation
    /// (higher = arrow grows faster with speed)
    static constexpr float ARROW_SPEED_SCALE    = 0.15f;

    /// Minimum arrow length in pixels
    static constexpr float ARROW_MIN_LENGTH     = 50.0f;

    /// Maximum arrow length in pixels
    static constexpr float ARROW_MAX_LENGTH     = 80.0f;

    // ========================================================================
    // Member Variables
    // ========================================================================

    /// Current position in world space (pixels)
    Math::Vec2f position;

    /// Current velocity vector (pixels per second)
    Math::Vec2f velocity;

    /// Ball radius in pixels (used for collision and rendering)
    float       radius;

    /// Ball color used for rendering
    Color       color;

    /// Whether the ball is currently moving (false = sleeping/stopped)
    bool        isMoving = true;

    // ========================================================================
    // Constructor
    // ========================================================================

    /**
     * @brief Constructs a Ball with initial properties.
     *
     * @param pos Initial position in world space
     * @param vel Initial velocity vector
     * @param r   Ball radius in pixels
     * @param c   Ball color for rendering
     */
    Ball(Math::Vec2f pos, Math::Vec2f vel, float r, Color c)
        : position{pos}, velocity{vel}, radius{r}, color{c} {}

    // ========================================================================
    // Physics and Collision
    // ========================================================================

    /**
     * @brief Handles collision with screen boundaries.
     *
     * Checks if the ball has left the screen bounds and reflects it back while
     * applying damping to the velocity. If the ball is moving very slowly after
     * hitting the bottom boundary, it stops completely.
     *
     * @param screenWidth  Screen width in pixels
     * @param screenHeight Screen height in pixels
     *
     * @note This modifies position and velocity in place.
     * @note Bottom collision has special handling: ball stops if velocity is below STOP_THRESHOLD
     */
    void HandleBoundaryCollision(int screenWidth, int screenHeight);

    // ========================================================================
    // Rendering
    // ========================================================================

    /**
     * @brief Renders the ball with all visual effects.
     *
     * Draws the ball as a filled circle with:
     * - Outer glow aura that pulses based on velocity
     * - Main ball body with solid color
     * - Highlight reflection
     * - Optional velocity direction arrow
     *
     * @param showVelocityArrow Whether to display the velocity indicator arrow
     *
     * @note Rendering is affected by Time::GetTime() for animation effects.
     * @see DrawVelocityArrow() for arrow implementation details
     */
    void Draw(bool showVelocityArrow = true) const;

private:
    // ========================================================================
    // Private Rendering Methods
    // ========================================================================

    /**
     * @brief Draws a direction arrow indicating velocity and speed.
     *
     * Renders an arrow from the ball center pointing in the direction of motion.
     * Arrow properties are determined by velocity:
     * - Length: Proportional to speed (scaled and clamped)
     * - Color: Gradient from cool to warm colors based on speed
     * - Style: Triangular arrowhead with fading shaft
     *
     * The arrow provides visual feedback for speed and direction without requiring
     * additional UI elements.
     *
     * @note Only called if velocity magnitude > 10.0f
     * @note Arrow is not drawn if ball is nearly stationary
     */
    void DrawVelocityArrow(float speed) const;
};

} // namespace RS
