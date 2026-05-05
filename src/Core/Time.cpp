#include "pch.hpp"
#include "Raysim/Core/Time.hpp"

#include <chrono>
#include <thread>

namespace RS {

namespace {

//============================================================
// Constants
//============================================================

constexpr float MIN_TIME_SCALE       = 0.01f;
constexpr float MIN_FIXED_DELTA_TIME = 0.0001f;
constexpr float MIN_MAX_DELTA        = 0.001f;

constexpr float EMA_ALPHA            = 0.05f;
constexpr int   FPS_WINDOW           = 30;     // slots in the rolling-average buffer
constexpr float FPS_WINDOW_SECS      = 0.5f;   // window spans 0.5s (same as Raylib)
constexpr float FPS_STEP             = FPS_WINDOW_SECS / FPS_WINDOW; // ~16.67ms per slot

//============================================================
// Types
//============================================================

using clock = std::chrono::steady_clock;
using seconds = std::chrono::duration<float>;

//============================================================
// Internal state container
//============================================================

struct TimeState
{
    clock::time_point realtimeStart = clock::now();
    clock::time_point frameStart    = clock::now();
    clock::time_point nextFrameTarget{};

    float frameTime        = 0.0f;
    float deltaTime        = 0.0f;
    float smoothDeltaTime  = 0.0f;

    float unscaledTotal    = 0.0f;
    float scaledTotal      = 0.0f;

    float timeScale        = 1.0f;
    float maximumDeltaTime = 0.25f;

    float fixedDeltaTime   = 0.02f;
    float fixedAccumulator = 0.0f;

    bool  paused           = false;

    float targetFrameTime  = 0.0f;

    // Rolling-average FPS (circular buffer of frame times, Raylib-style time-gated)
    float fpsHistory[FPS_WINDOW] = {};
    float fpsHistorySum           = 0.0f;
    int   fpsHistoryIndex         = 0;
    int   fpsHistoryCount         = 0;
    float fpsNextStep             = FPS_STEP;  // unscaledTotal threshold for next slot advance

    float smoothedFPS      = 0.0f;
    int   currentFPS       = 0;   // updated once per second
    float fpsTimer         = 0.0f;
    int   framesSec        = 0;

    uint64_t frameCount    = 0;
};

TimeState g;

//============================================================
// Utility functions
//============================================================

inline float TimeSince(const clock::time_point& t)
{
    return seconds(clock::now() - t).count();
}

inline void UpdateEMA(float sample, float& ema)
{
    if (ema == 0.0f)
        ema = sample;
    else
        ema = (1.0f - EMA_ALPHA) * ema + EMA_ALPHA * sample;
}

void UpdateFPS(float frameTime)
{
    if (frameTime <= 0.0f) return;

    // This makes the window always cover FPS_WINDOW_SECS (0.5s) of real time
    while (g.unscaledTotal >= g.fpsNextStep)
    {
        g.fpsNextStep += FPS_STEP;

        g.fpsHistorySum -= g.fpsHistory[g.fpsHistoryIndex];
        g.fpsHistory[g.fpsHistoryIndex] = frameTime;
        g.fpsHistorySum += g.fpsHistory[g.fpsHistoryIndex];

        g.fpsHistoryIndex = (g.fpsHistoryIndex + 1) % FPS_WINDOW;

        if (g.fpsHistoryCount < FPS_WINDOW)
            g.fpsHistoryCount++;
    }

    if (g.fpsHistoryCount == 0) return;

    const float avgFrameTime = g.fpsHistorySum / static_cast<float>(g.fpsHistoryCount);
    const float instantFPS   = 1.0f / avgFrameTime;
    UpdateEMA(instantFPS, g.smoothedFPS);

    // currentFPS updates once per second for a stable integer readout
    g.fpsTimer   += frameTime;
    g.framesSec++;
    if (g.fpsTimer >= 1.0f)
    {
        g.currentFPS  = g.framesSec;
        g.framesSec   = 0;
        g.fpsTimer   -= 1.0f;
    }
}

void LimitFPS()
{
    if (g.targetFrameTime <= 0.0f)
        return;

    // Sleep until ~1ms before target, then busy-spin the remainder for precision.
    const auto sleepUntil = g.nextFrameTarget - std::chrono::milliseconds(1);
    if (clock::now() < sleepUntil)
        std::this_thread::sleep_until(sleepUntil);

    while (clock::now() < g.nextFrameTarget)
        std::this_thread::yield();

    // If we overslept by more than one full frame, reset the cadence to avoid
    // a burst of back-to-back frames trying to catch up.
    const auto now = clock::now();
    if (now > g.nextFrameTarget + std::chrono::duration_cast<clock::duration>(seconds(g.targetFrameTime)))
        g.nextFrameTarget = now;

    g.nextFrameTarget += std::chrono::duration_cast<clock::duration>(
        seconds(g.targetFrameTime));
}

} // anonymous namespace

//============================================================
// Frame lifecycle
//============================================================

void Time::BeginFrame()
{
    g.frameStart = clock::now();
}

void Time::EndFrame()
{
    LimitFPS();

    g.frameTime = TimeSince(g.frameStart);
    g.deltaTime = g.paused ? 0.0f : std::min(g.frameTime, g.maximumDeltaTime);

    const float scaledDelta = g.deltaTime * g.timeScale;

    g.unscaledTotal += g.deltaTime;
    g.scaledTotal   += scaledDelta;
    g.frameCount++;

    UpdateFPS(g.frameTime);
    UpdateEMA(g.deltaTime, g.smoothDeltaTime);

    g.fixedAccumulator += g.deltaTime;
    g.fixedAccumulator = std::min(g.fixedAccumulator, g.maximumDeltaTime);
}

//============================================================
// Pause
//============================================================

void Time::Pause()           { g.paused = true; }
void Time::Resume()          { g.paused = false; }
bool Time::IsPaused()        { return g.paused; }

//============================================================
// Fixed update
//============================================================

bool Time::ShouldFixedStep()
{
    if (g.fixedAccumulator >= g.fixedDeltaTime)
    {
        g.fixedAccumulator -= g.fixedDeltaTime;
        return true;
    }

    return false;
}

float Time::GetInterpolationAlpha()
{
    if (g.fixedDeltaTime <= 0.0f)
        return 0.0f;

    return g.fixedAccumulator / g.fixedDeltaTime;
}

float Time::GetFixedDeltaTime()
{
    return g.fixedDeltaTime * g.timeScale;
}

float Time::GetFixedUnscaledDeltaTime()
{
    return g.fixedDeltaTime;
}

void Time::SetFixedDeltaTime(float step)
{
    g.fixedDeltaTime = std::max(step, MIN_FIXED_DELTA_TIME);
}

//============================================================
// Delta time
//============================================================

float Time::GetFrameTime()               { return g.frameTime; }
float Time::GetDeltaTime()               { return g.deltaTime * g.timeScale; }
float Time::GetUnscaledDeltaTime()       { return g.deltaTime; }
float Time::GetSmoothDeltaTime()         { return g.smoothDeltaTime * g.timeScale; }
float Time::GetUnscaledSmoothDeltaTime() { return g.smoothDeltaTime; }

//============================================================
// Time scale
//============================================================

float Time::GetTimeScale() { return g.timeScale; }

void Time::SetTimeScale(float scale)
{
    g.timeScale = std::max(scale, MIN_TIME_SCALE);
}

//============================================================
// Elapsed time
//============================================================

float Time::GetTime()         { return g.scaledTotal; }
float Time::GetUnscaledTime() { return g.unscaledTotal; }

float Time::GetRealtimeSinceStartup()
{
    return TimeSince(g.realtimeStart);
}

//============================================================
// FPS
//============================================================

int Time::GetFPS()
{
    return g.currentFPS;
}

float Time::GetSmoothedFPS()
{
    return g.smoothedFPS;
}

void Time::SetTargetFPS(int fps)
{
    if (fps > 0)
    {
        g.targetFrameTime = 1.0f / static_cast<float>(fps);

        g.nextFrameTarget = clock::now() +
            std::chrono::duration_cast<clock::duration>(
                seconds(g.targetFrameTime));
    }
    else
    {
        g.targetFrameTime = 0.0f;
    }
}

//============================================================
// Configuration
//============================================================

float Time::GetMaximumDeltaTime()
{
    return g.maximumDeltaTime;
}

void Time::SetMaximumDeltaTime(float maxDelta)
{
    g.maximumDeltaTime = std::max(maxDelta, MIN_MAX_DELTA);
}

//============================================================
// Frame count
//============================================================

uint64_t Time::GetFrameCount()
{
    return g.frameCount;
}

//============================================================
// Utility
//============================================================

void Time::Reset()
{
    auto start = g.realtimeStart;
    g = TimeState{};
    g.realtimeStart = start;
}

} // namespace RS
