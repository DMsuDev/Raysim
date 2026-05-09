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

/**********************************************************************************************
 *   Raysim Math Utilities
 *
 *   Provides common mathematical constants and stateless utility functions:
 *     - Angle conversion  (ToRadians, ToDegrees)
 *     - Clamping          (Clamp, Clamp01, MinValue, MaxValue)
 *     - Sign / absolute   (Sign, Abs)
 *     - Interpolation     (Lerp, LerpUnclamped, InverseLerp, Remap, MapRange)
 *     - Smoothing         (SmoothStep01, SmootherStep01, SmoothDamp)
 *     - Stepping          (MoveTowards, Repeat, PingPong)
 *     - Angular helpers   (NormalizeAngle, DeltaAngle)
 *     - Comparison        (EpsilonEquals)
 *     - Algebraic         (Sqrt, Sqr, Floor, FloorToInt, Fract, Saturate)
 *
 *   This header is included by `Math.hpp` and re-exported as part of the public Math API.
 **********************************************************************************************/

#pragma once

#include <concepts>

namespace RS::Math {

//==============================================================================
// Constants
//==============================================================================

    /// Mathematical constant pi (~= 3.14159...).
    constexpr float PI      = 3.14159265358979323846f;
    /// 2*pi (~= 6.28318...) - full revolution in radians.
    constexpr float TWO_PI  = 2.0f * PI;
    /// pi/2 (~= 1.5708...) - quarter revolution in radians.
    constexpr float HALF_PI = PI * 0.5f;
    /// Multiply by this to convert degrees -> radians (pi / 180).
    constexpr float DEG2RAD = PI / 180.0f;
    /// Multiply by this to convert radians -> degrees (180 / pi).
    constexpr float RAD2DEG = 180.0f / PI;

//==============================================================================
// Angle conversion
//==============================================================================

    /**
     * @brief Converts an angle from degrees to radians.
     * @param degrees Angle value in degrees.
     * @return Equivalent angle in radians.
     */
    [[nodiscard]] constexpr float ToRadians(float degrees) noexcept;

    /**
     * @brief Converts an angle from radians to degrees.
     * @param radians Angle value in radians.
     * @return Equivalent angle in degrees.
     */
    [[nodiscard]] constexpr float ToDegrees(float radians) noexcept;

//==============================================================================
// Clamping and extrema
//==============================================================================

    /**
     * @brief Clamps `value` to the inclusive range [`minValue`, `maxValue`].
     *
     * If `value` is below `minValue`, `minValue` is returned. If it exceeds
     * `maxValue`, `maxValue` is returned. Otherwise, `value` is returned as-is.
     *
     * @tparam T Comparable type.
     * @param value    Value to clamp.
     * @param minValue Lower bound of the clamp range.
     * @param maxValue Upper bound of the clamp range.
     * @return `value` clamped to [`minValue`, `maxValue`].
     */
    template <typename T>
    [[nodiscard]] constexpr T Clamp(T value, T minValue, T maxValue) noexcept;

    /**
     * @brief Clamps `value` to the range [0, 1].
     *
     * Equivalent to `Clamp(value, 0.0f, 1.0f)`.
     *
     * @param value Value to clamp.
     * @return `value` clamped to [0, 1].
     */
    [[nodiscard]] constexpr float Clamp01(float value) noexcept;

    /**
     * @brief Returns the smaller of two values.
     * @tparam T Comparable type.
     * @param a First value.
     * @param b Second value.
     * @return `a` if `a < b`, otherwise `b`.
     */
    template <typename T>
    [[nodiscard]] constexpr T MinValue(T a, T b) noexcept;

    /**
     * @brief Returns the larger of two values.
     * @tparam T Comparable type.
     * @param a First value.
     * @param b Second value.
     * @return `a` if `a > b`, otherwise `b`.
     */
    template <typename T>
    [[nodiscard]] constexpr T MaxValue(T a, T b) noexcept;

//==============================================================================
// Sign and absolute value
//==============================================================================

    /**
     * @brief Returns the sign of `value` as -1, 0, or +1.
     *
     * Uses the branchless three-way expression `(T{} < value) - (value < T{})`.
     *
     * @tparam T Arithmetic type.
     * @param value Value whose sign is queried.
     * @return -1 if `value < 0`, 0 if `value == 0`, or +1 if `value > 0`.
     */
    template <typename T>
    [[nodiscard]] constexpr int Sign(T value) noexcept;

    /// Constrains `Abs` to signed integral and floating-point types.
    template <typename T>
    concept AbsType = std::signed_integral<T> || std::floating_point<T>;

    /**
     * @brief Returns the absolute value of `value`.
     * @tparam T Signed integral or floating-point type (see `AbsType`).
     * @param value Value to compute the absolute value of.
     * @return `value` if non-negative, `-value` otherwise.
     */
    template <AbsType T>
    [[nodiscard]] constexpr T Abs(T value) noexcept;

//==============================================================================
// Interpolation and remapping
//==============================================================================

    /**
     * @brief Linearly interpolates between `a` and `b` by factor `t`.
     *
     * `t` is clamped to [0, 1], so the result is always within [`a`, `b`].
     *
     * @tparam T Arithmetic type.
     * @param a Start value (returned when `t == 0`).
     * @param b End value (returned when `t == 1`).
     * @param t Interpolation factor, clamped to [0, 1].
     * @return Interpolated value between `a` and `b`.
     */
    template <typename T>
    [[nodiscard]] constexpr T Lerp(T a, T b, float t) noexcept;

    /**
     * @brief Linearly interpolates between `a` and `b` without clamping `t`.
     *
     * Unlike `Lerp`, `t` is not restricted to [0, 1], so the result may fall
     * outside [`a`, `b`].
     *
     * @tparam T Arithmetic type.
     * @param a Start value.
     * @param b End value.
     * @param t Unclamped interpolation factor.
     * @return Extrapolated or interpolated value along the `a`-`b` line.
     */
    template <typename T>
    [[nodiscard]] constexpr T LerpUnclamped(T a, T b, float t) noexcept;

    /**
     * @brief Computes the inverse of a linear interpolation.
     *
     * Returns `t` in [0, 1] such that `LerpUnclamped(a, b, t) ~= value`.
     * If `a` and `b` are too close (|b - a| <= 1e-12), returns 0.
     *
     * @param a     Range start.
     * @param b     Range end.
     * @param value Value to locate within the range.
     * @return Normalized position of `value` in [0, 1].
     */
    [[nodiscard]] float InverseLerp(float a, float b, float value) noexcept;

    /**
     * @brief Remaps `value` from [inMin, inMax] to [outMin, outMax].
     *
     * Equivalent to `LerpUnclamped(outMin, outMax, InverseLerp(inMin, inMax, value))`.
     * The input is clamped at the boundaries of [inMin, inMax].
     *
     * @param value  Value to remap.
     * @param inMin  Lower bound of the input range.
     * @param inMax  Upper bound of the input range.
     * @param outMin Lower bound of the output range.
     * @param outMax Upper bound of the output range.
     * @return `value` mapped to [outMin, outMax].
     */
    [[nodiscard]] float Remap(
        float value, float inMin, float inMax, float outMin, float outMax) noexcept;

    /**
     * @brief Remaps `value` linearly from [inMin, inMax] to [outMin, outMax] without clamping.
     *
     * Unlike `Remap`, inputs outside [inMin, inMax] produce outputs outside [outMin, outMax].
     *
     * @param value  Value to remap.
     * @param inMin  Lower bound of the input range.
     * @param inMax  Upper bound of the input range.
     * @param outMin Lower bound of the output range.
     * @param outMax Upper bound of the output range.
     * @return `value` linearly mapped from the input to the output range.
     */
    [[nodiscard]] constexpr float MapRange(
        float value, float inMin, float inMax, float outMin, float outMax) noexcept;

    /**
     * @brief Smoothly damps `current` towards `target` over time.
     *
     * Implements the industry-standard critically-damped spring approximation.
     * `currentVelocity` tracks the moving velocity and must be preserved between
     * calls (initialize it to 0).
     *
     * @param current         Current value.
     * @param target          Target value to approach.
     * @param currentVelocity In/out velocity state; updated each call.
     * @param smoothTime      Approximate time (seconds) to reach `target`.
     * @param deltaTime       Time elapsed since the last call (seconds).
     * @return New value smoothly moved one step towards `target`.
     */
    [[nodiscard]] float SmoothDamp(
        float current, float target, float& currentVelocity,
        float smoothTime, float deltaTime) noexcept;

    /**
     * @brief Smooth Hermite interpolation from 0 to 1 as `t` goes from 0 to 1.
     *
     * Applies the cubic polynomial `3t^2 - 2t^3` after clamping `t` to [0, 1],
     * producing a smooth S-curve commonly used for easing animations.
     *
     * @param t Input factor, clamped to [0, 1].
     * @return Smooth value in [0, 1].
     */
    [[nodiscard]] float SmoothStep01(float t) noexcept;

    /**
     * @brief Quintic (smoother) Hermite interpolation from 0 to 1 as `t` goes from 0 to 1.
     *
     * Applies the polynomial `6t^5 - 15t^4 + 10t^3` after clamping `t` to [0, 1].
     * Unlike `SmoothStep01`, the first- and second-order derivatives are both zero
     * at t=0 and t=1, producing even smoother transitions with no perceptible
     * acceleration discontinuity. This is the fade curve used in Perlin and
     * Simplex noise.
     *
     * @param t Input factor, clamped to [0, 1].
     * @return Smooth value in [0, 1].
     */
    [[nodiscard]] constexpr float SmootherStep01(float t) noexcept;

//==============================================================================
// Stepping and movement
//==============================================================================

    /**
     * @brief Moves `current` towards `target` by at most `maxDelta`.
     *
     * If the distance is already within `maxDelta`, `target` is returned exactly.
     *
     * @param current  Starting value.
     * @param target   Destination value.
     * @param maxDelta Maximum step size (must be non-negative).
     * @return New value after stepping towards `target`.
     */
    [[nodiscard]] float MoveTowards(float current, float target, float maxDelta) noexcept;

    /**
     * @brief Wraps `t` so that it repeats within [0, `length`).
     *
     * Equivalent to a floating-point modulo, handling negative `t` correctly.
     * Returns 0 when `length` <= 0.
     *
     * @param t      Input value.
     * @param length Length of the repeating interval (must be positive).
     * @return `t` wrapped to [0, `length`).
     */
    [[nodiscard]] float Repeat(float t, float length) noexcept;

    /**
     * @brief Ping-pongs `t` so that it bounces between 0 and `length`.
     *
     * The value increases from 0 to `length`, then decreases back to 0, and
     * so on, producing triangular oscillation without an explicit sine.
     *
     * @param t      Input value (typically elapsed time).
     * @param length Peak value before direction reverses.
     * @return Value oscillating between 0 and `length`.
     */
    [[nodiscard]] float PingPong(float t, float length) noexcept;

//==============================================================================
// Angular helpers
//==============================================================================

    /**
     * @brief Normalizes `angle` to the range [0, 2*pi).
     *
     * Uses `std::fmod` to bring any angle into the unsigned canonical range.
     * Negative angles are mapped to their positive equivalent.
     *
     * @param angle Angle in radians (any value).
     * @return Equivalent angle in [0, 2*pi).
     */
    [[nodiscard]] float NormalizeAngle(float angle) noexcept;

    /**
     * @brief Computes the shortest signed angular difference from `a` to `b`.
     *
     * The result is wrapped to (-pi, pi] so it always represents the shortest
     * signed arc between the two angles. A positive result means `b` is
     * counter-clockwise from `a`.
     *
     * @param a Source angle in radians.
     * @param b Target angle in radians.
     * @return Signed difference in (-pi, pi].
     */
    [[nodiscard]] float DeltaAngle(float a, float b) noexcept;

//==============================================================================
// Comparison
//==============================================================================

    /**
     * @brief Checks whether two floats are approximately equal within `epsilon`.
     *
     * Returns `true` if `|a - b| <= epsilon`. The default tolerance `1e-6f` is
     * appropriate for most single-precision comparisons.
     *
     * @param a       First value.
     * @param b       Second value.
     * @param epsilon Maximum allowed absolute difference (default: `1e-6f`).
     * @return `true` if `|a - b|` is within `epsilon`.
     */
    [[nodiscard]] constexpr bool EpsilonEquals(float a, float b, float epsilon = 1e-6f) noexcept;

//==============================================================================
// Algebraic helpers
//==============================================================================

    /**
     * @brief Computes the square root of `x`.
     *
     * Thin wrapper around `std::sqrt` provided for namespace consistency.
     *
     * @param x Non-negative input value.
     * @return Square root of `x`.
     */
    [[nodiscard]] float Sqrt(float x) noexcept;

    /**
     * @brief Computes the square of `x` (`x * x`).
     * @param x Input value.
     * @return `x^2`.
     */
    [[nodiscard]] constexpr float Sqr(float x) noexcept;

    /**
     * @brief Computes the floor of `x` (largest integer not greater than `x`).
     *
     * Thin wrapper around `std::floor` provided for namespace consistency.
     *
     * @param x Input value.
     * @return The largest integer value that is not greater than `x`.
     */
    [[nodiscard]] float Floor(float x) noexcept;

    /**
     * @brief Computes `floor(x)` and returns the result as an `int`.
     *
     * Equivalent to `static_cast<int>(std::floor(x))`. Useful for converting
     * a floating-point coordinate to its integer grid cell index while correctly
     * handling negative values (unlike a plain truncating cast).
     *
     * @param x Input value.
     * @return `floor(x)` cast to `int`.
     */
    [[nodiscard]] int FloorToInt(float x) noexcept;

    /**
     * @brief Returns the fractional part of `x`.
     *
     * Defined as `x - floor(x)`, so the result is always in [0, 1).
     * For negative values, e.g. `-1.3f`, the result is `0.7f`.
     *
     * @param x Input floating-point value.
     * @return Fractional part of `x` in [0, 1).
     */
    [[nodiscard]] constexpr float Fract(float x) noexcept;

    /**
     * @brief Clamps `x` to [0, 1].
     *
     * GPU-style alias for `Clamp01`, provided for convenience when porting
     * shader-like math.
     *
     * @param x Input value.
     * @return `x` clamped to [0, 1].
     */
    [[nodiscard]] constexpr float Saturate(float x) noexcept;

} // namespace RS::Math

#include "Raysim/Math/Utils/MathUtils.inl" // NOLINT(misc-include-in-implementation)
