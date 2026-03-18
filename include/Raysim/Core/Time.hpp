#pragma once

#include <cstdint>

namespace RS {

/**
 * @brief Global time utility.
 *
 * Provides frame timing, fixed timestep simulation,
 * time scaling and FPS statistics.
 */
class Time
{
public:

    Time() = delete;

    //============================================================
    // Frame Lifecycle
    //============================================================

    /// Marks the beginning of a new frame.
    static void BeginFrame();

    /// Finalizes frame timing, updates FPS counters and applies frame limiting.
    static void EndFrame();

    //============================================================
    // Pause
    //============================================================

    /**
     * @brief Pauses the simulation.
     *
     * While paused, `GetDeltaTime()` returns 0, the fixed accumulator does not
     * advance, and elapsed totals are frozen. FPS counters and
     * `GetRealtimeSinceStartup()` continue normally.
     */
    static void Pause();

    /// Resumes a previously paused simulation.
    static void Resume();

    /// Returns true if the simulation is currently paused.
    static bool IsPaused();

    //============================================================
    // Fixed Update (Deterministic Simulation)
    //============================================================

    /**
     * @brief Consumes a fixed timestep from the accumulator.
     *
     * Returns true while fixed updates should continue running.
     *
     * Typical usage:
     * @code
     * int steps = 0;
     * while (Time::FixedStep() && steps < MAX_FIXED_STEPS)
     * {
     *     FixedUpdate(Time::GetFixedDeltaTime());
     *     steps++;
     * }
     * @endcode
     */
    static bool ShouldFixedStep();

    /**
     * @brief Fractional progress through the current fixed timestep, in [0, 1).
     *
     * After the fixed-step loop, the accumulator still holds the leftover time
     * that did not fill a complete fixed step.  Dividing that remainder by the
     * fixed step size yields a value between 0 and 1 that represents how far
     * the simulation has advanced *into* the next tick.  Use it to linearly
     * interpolate (lerp) rendered state between the previous and current
     * physics snapshot so that visuals stay smooth at any frame rate.
     *
     * Example usage in Draw():
     * @code
     * float alpha  = Time::GetInterpolationAlpha();
     * Vector2 renderPos = Lerp(prevState.position, currentState.position, alpha);
     * DrawObject(renderPos);
     * @endcode
     *
     * @note Returns 0 when fixedDeltaTime is zero (guard against division by zero).
     * @return Alpha in [0, 1).
     */
    static float GetInterpolationAlpha();

    /// Fixed timestep (scaled by timeScale).
    static float GetFixedDeltaTime();

    /// Fixed timestep unaffected by timeScale.
    static float GetFixedUnscaledDeltaTime();

    /// Sets the fixed timestep interval (seconds).
    static void SetFixedDeltaTime(float step);

    //============================================================
    // Frame Delta Time
    //============================================================

    /// Time elapsed since last frame (affected by timeScale).
    static float GetDeltaTime();

    /// Time elapsed since last frame (ignores timeScale).
    static float GetUnscaledDeltaTime();

    /// Smoothed delta time using exponential moving average (EMA).
    static float GetSmoothDeltaTime();

    /// Raw measured frame duration (before clamping/scaling).
    static float GetFrameTime();

    //============================================================
    // Time Scale
    //============================================================

    /// Current time scale (1.0 = real time).
    static float GetTimeScale();

    /// Sets the global time scale.
    static void SetTimeScale(float scale);

    //============================================================
    // Elapsed Time
    //============================================================

    /// Total game time (affected by timeScale).
    static float GetTime();

    /// Total real elapsed time (ignores timeScale).
    static float GetUnscaledTime();

    /// Real time since application start.
    static float GetRealtimeSinceStartup();

    //============================================================
    // Frame Rate
    //============================================================

    /// Frames per second measured over the last second.
    static int GetFPS();

    /// Smoothed FPS using exponential moving average.
    static float GetSmoothedFPS();

    /// Limits the frame rate (0 = unlimited).
    static void SetTargetFPS(int fps);

    //============================================================
    // Configuration
    //============================================================

    /// Maximum allowed delta time (prevents large spikes).
    static float GetMaximumDeltaTime();

    /// Sets the maximum allowed delta time.
    static void SetMaximumDeltaTime(float maxDelta);

    //============================================================
    // Statistics
    //============================================================

    /// Total frames rendered since startup.
    static uint64_t GetFrameCount();

    // =============================================================
    // Utility
    // =============================================================

    /// Resets all timing values to their initial state. Useful for testing or restarting the application.
    static void Reset();
};

} // namespace RS
