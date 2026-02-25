#include "Raysim/Manager/TimeManager.hpp"

#include <raylib.h>
#include <cmath>

namespace RS {

// ============================================================================
// FPS MANAGEMENT
// ============================================================================

void TimeManager::SetTargetFPS(int fps) {
    targetFps_ = fps;
    ::SetTargetFPS(fps);
}

int TimeManager::GetFPS() const {
    return ::GetFPS();
}

float TimeManager::GetFrameTime() const {
    return ::GetFrameTime();
}

float TimeManager::GetTime() const {
    return static_cast<float>(::GetTime());
}

float TimeManager::GetDeltaTime() const {
    return GetFrameTime();
}

// ============================================================================
// TIMING QUERIES
// ============================================================================

bool TimeManager::HasTimeElapsed(float seconds) const {
    return GetTime() >= seconds;
}

bool TimeManager::IsInterval(float interval) const {
    if (interval <= 0.0f) return false;

    float currentTime = GetTime();
    float currentPosition = std::fmod(currentTime, interval);
    float previousTime = currentTime - GetFrameTime();
    float previousPosition = std::fmod(previousTime, interval);

    // True if we crossed an interval boundary this frame
    return previousPosition > currentPosition;
}

// ============================================================================
// UTILITY
// ============================================================================

void TimeManager::Update() {
    frameCount_++;
}

void TimeManager::Reset() {
    frameCount_ = 0;
    lastIntervalTime_ = 0.0f;
}

} // namespace RS
