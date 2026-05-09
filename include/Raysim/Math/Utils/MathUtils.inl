/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#include "Raysim/Math/Utils/MathUtils.hpp"   // NOLINT(misc-header-include-cycle)

#include <cmath>

namespace RS::Math {

//==============================================================================
// Angle conversion
//==============================================================================

constexpr float ToRadians(float degrees) noexcept { return degrees * DEG2RAD; }
constexpr float ToDegrees(float radians) noexcept { return radians * RAD2DEG; }

//==============================================================================
// Clamping and extrema
//==============================================================================

template <typename T>
constexpr T Clamp(T value, T minValue, T maxValue) noexcept
{
    return (value < minValue) ? minValue : (value > maxValue) ? maxValue : value;
}

constexpr float Clamp01(float value) noexcept { return Clamp(value, 0.0f, 1.0f); }

template <typename T>
constexpr T MinValue(T a, T b) noexcept { return (a < b) ? a : b; }

template <typename T>
constexpr T MaxValue(T a, T b) noexcept { return (a > b) ? a : b; }

//==============================================================================
// Sign and absolute value
//==============================================================================

template <typename T>
constexpr int Sign(T value) noexcept
{
    return (T{} < value) - (value < T{});
}

template <AbsType T>
constexpr T Abs(T value) noexcept { return (value < T{0}) ? -value : value; }

//==============================================================================
// Interpolation and remapping
//==============================================================================

template <typename T>
constexpr T Lerp(T a, T b, float t) noexcept
{
    t = Clamp01(t);
    return static_cast<T>(a + (b - a) * t);
}

template <typename T>
constexpr T LerpUnclamped(T a, T b, float t) noexcept
{
    return static_cast<T>(a + (b - a) * t);
}

inline float InverseLerp(float a, float b, float value) noexcept
{
    const float denom = b - a;
    return (std::fabs(denom) <= 1e-12f) ? 0.0f : Clamp01((value - a) / denom);
}

inline float Remap(float value, float inMin, float inMax, float outMin, float outMax) noexcept
{
    return LerpUnclamped(outMin, outMax, InverseLerp(inMin, inMax, value));
}

constexpr float MapRange(float value, float inMin, float inMax, float outMin, float outMax) noexcept
{
    return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}

inline float SmoothDamp(
    float current, float target, float& currentVelocity,
    float smoothTime, float deltaTime) noexcept
{
    const float omega = 2.0f / smoothTime;
    const float k     = omega * deltaTime;
    const float exp   = 1.0f / (1.0f + k + 0.48f * k * k + 0.235f * k * k * k);

    const float change = current - target;
    const float temp   = (currentVelocity + omega * change) * deltaTime;
    currentVelocity    = (currentVelocity - omega * temp) * exp;

    return target + (change + temp) * exp;
}

inline float SmoothStep01(float t) noexcept
{
    t = Clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}

constexpr float SmootherStep01(float t) noexcept
{
    t = Clamp01(t);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

//==============================================================================
// Stepping and movement
//==============================================================================

inline float MoveTowards(float current, float target, float maxDelta) noexcept
{
    if (std::fabs(target - current) <= maxDelta) return target;
    return current + (target > current ? maxDelta : -maxDelta);
}

inline float Repeat(float t, float length) noexcept
{
    if (length <= 0.0f) return 0.0f;
    return t - std::floor(t / length) * length;
}

inline float PingPong(float t, float length) noexcept
{
    const float r = Repeat(t, length * 2.0f);
    return length - std::fabs(r - length);
}

//==============================================================================
// Angular helpers
//==============================================================================

inline float NormalizeAngle(float angle) noexcept
{
    angle = std::fmod(angle, TWO_PI);
    return (angle < 0.0f) ? angle + TWO_PI : angle;
}

inline float DeltaAngle(float a, float b) noexcept
{
    float diff = std::fmod(b - a, TWO_PI);
    if (diff >  PI) diff -= TWO_PI;
    if (diff < -PI) diff += TWO_PI;
    return diff;
}

//==============================================================================
// Comparison
//==============================================================================

constexpr bool EpsilonEquals(float a, float b, float epsilon) noexcept
{
    return std::fabs(a - b) <= epsilon;
}

//==============================================================================
// Algebraic helpers
//==============================================================================

inline float Sqrt(float x) noexcept { return std::sqrt(x); }
constexpr float Sqr(float x) noexcept { return x * x; }

inline float Floor(float x) noexcept { return std::floor(x); }
inline int FloorToInt(float x) noexcept { return static_cast<int>(std::floor(x)); }

constexpr float Fract(float x) noexcept { return x - std::floor(x); }
constexpr float Saturate(float x) noexcept { return Clamp(x, 0.0f, 1.0f); }

} // namespace RS::Math
