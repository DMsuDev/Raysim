#pragma once

namespace RS {

/**
 * @class TimeManager
 * @brief Comprehensive timing and frame rate management system
 *
 * Manages all time-related aspects of the application including:
 *
 * - FPS Control: Set and monitor frame rate
 *
 * - Delta Time: Query time elapsed since last frame (essential for frame-rate independent gameplay)
 *
 * - Frame Counting: Track total frames rendered
 *
 * - Interval Checks: Efficiently check if periodic updates should occur
 *
 * - Elapsed Time: Get total time since application started
 *
 * All time values are in seconds by default; millisecond versions are available when needed.
 * This manager is automatically updated each frame by the Canvas.
 *
 * @example
 * TimeManager& time = canvas.GetTimeManager();
 *
 * // Frame-rate independent movement
 * position += velocity * time.GetDeltaTime();
 *
 * // Periodic event (e.g., every 0.5 seconds)
 * if (time.IsInterval(0.5f)) {
 *     SpawnEnemy();
 * }
 *
 * @see Canvas::Run(), GetDeltaTime()
 */
class TimeManager {
public:
    TimeManager() = default;
    ~TimeManager() = default;

    // ============================================================================
    // FPS MANAGEMENT
    // ============================================================================

    /**
     * @brief Set the target frame rate (frames per second)
     *
     * Configures the maximum frame rate for the application. The engine
     * will attempt to maintain this FPS by limiting frame generation.
     *
     * @param fps Target frames per second (e.g., 60, 120, 144)
     * @note Common values are 30, 60, 120; higher values require more processing power
     * @note Setting to 0 disables frame rate limiting (unlimited FPS)
     * @see GetTargetFPS(), GetCurrentFPS()
     */
    void SetTargetFPS(int fps);

    /**
     * @brief Get the configured target frame rate
     * @return Target FPS value set via SetTargetFPS()
     */
    int GetTargetFPS() const { return targetFps_; }

    /**
     * @brief Get the actual measured frames per second
     *
     * Returns the real-time FPS being achieved by the application.
     * This may differ from the target FPS if the system can't keep up.
     *
     * @return Current measured FPS (averaged over recent frames)
     * @note Useful for performance monitoring and debugging
     */
    int GetCurrentFPS() const { return GetFPS(); }

    // ============================================================================
    // DELTA TIME & TIMING
    // ============================================================================

    /**
     * @brief Get elapsed time since the last frame in seconds
     *
     * Returns the delta time used for smooth motion and animation.
     * Multiply movement by delta time for frame-rate independent movement.
     *
     * @return Time elapsed since last frame in seconds (e.g., 0.0167f at 60 FPS)
     * @note Use this for smooth animation: position += velocity * GetDeltaTime()
     * @example
     * float velocity = 100.0f;
     * position.x += velocity * timeManager.GetDeltaTime();  // 100 pixels per second
     * @see GetDeltaTimeMs()
     */
    float GetDeltaTime() const;

    /**
     * @brief Get elapsed time since the last frame in milliseconds
     *
     * Same as GetDeltaTime() but expressed in milliseconds.
     * Convenient for timing code that uses millisecond units.
     *
     * @return Time elapsed since last frame in milliseconds (e.g., 16.67ms at 60 FPS)
     * @see GetDeltaTime()
     */
    float GetDeltaTimeMs() const { return GetFrameTime() * 1000.0f; }

    /**
     * @brief Get total elapsed time since application start in seconds
     *
     * Returns cumulative time since the engine initialized.
     * Useful for animation timers, effects duration, and game state transitions.
     *
     * @return Total seconds elapsed since initialization (high precision microseconds)
     * @note This counter includes all frames, paused or not
     * @example
     * if (timeManager.GetElapsedTime() > 5.0f)  // After 5 seconds
     * {
     *     levelComplete = true;
     * }
     */
    float GetElapsedTime() const { return GetTime(); }

    // ============================================================================
    // FRAME INFORMATION
    // ============================================================================

    /**
     * @brief Get the total number of frames rendered since start
     *
     * Increments by 1 each frame. Useful for periodic events and frame-based logic.
     *
     * @return Total frame count since application start
     * @note Frame 0 is the first frame of the application
     * @example
     * if (timeManager.GetFrameCount() % 60 == 0)  // Every 60 frames at 60 FPS
     * {
     *     // Execute once per second
     * }
     */
    int GetFrameCount() const { return frameCount_; }

    /**
     * @brief Check if a specific amount of time has elapsed since application start
     *
     * Useful for checking absolute time milestones (e.g., "has it been 5 seconds total?").
     * Resets only when Reset() is called.
     *
     * @param seconds Time threshold in seconds
     * @return true if GetElapsedTime() >= seconds
     * @note Useful for one-time events: if (!activated && time.HasTimeElapsed(3.0f)) { DoThing(); }
     * @see IsInterval(), GetElapsedTime()
     */
    bool HasTimeElapsed(float seconds) const;

    /**
     * @brief Check if we're within a repeating time interval
     *
     * Checks if the current elapsed time falls within a repeating interval.
     * Useful for periodic effects: damage ticks, projectile spawns, visual effects.
     *
     * @param interval Time interval in seconds (e.g., 0.5f for every half-second)
     * @return true if (GetElapsedTime() % interval) is within the current frame zone
     * @note Example: IsInterval(0.1f) fires approximately 10 times per second
     * @example
     * if (timeManager.IsInterval(2.0f))  // Every 2 seconds
     * {
     *     SpawnEnemy();
     * }
     * @see HasTimeElapsed(), GetFrameCount()
     */
    bool IsInterval(float interval) const;

    // ============================================================================
    // UTILITY
    // ============================================================================

    /**
     * @brief Update timing information each frame
     *
     * Called automatically by Canvas::Run() at the start of each frame.
     * Manages frame counting, interval tracking, and internal state updates.
     *
     * @note Required to be called once per frame for all timing functions to work correctly
     * @warning Do not call manually if using Canvas::Run() - it's called automatically
     * @see Canvas::Run()
     */
    void Update();

    /**
     * @brief Reset all timing counters
     *
     * Resets elapsed time and frame count to 0. Useful for level transitions,
     * state changes, or when timing context changes within the application.
     *
     * @note After calling Reset(), GetElapsedTime() and GetFrameCount() return 0
     * @note Does not affect target FPS or delta time calculations
     */
    void Reset();

private:
    int targetFps_ = 60;
    int frameCount_ = 0;
    float lastIntervalTime_ = 0.0f;

    // raylib internal functions
    void SetTargetFPS(int fps) const;
    int GetFPS() const;
    float GetFrameTime() const;
    float GetTime() const;
};

} // namespace RS
