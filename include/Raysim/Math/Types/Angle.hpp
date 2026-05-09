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

#pragma once

#include <ostream>

namespace RS::Math {

/**
 * @struct Angle
 * @brief Strongly-typed wrapper for angle values stored internally in radians.
 *
 * Prevents the common error of accidentally mixing radians and degrees by
 * requiring all construction to go through the explicit factory methods
 * `Radians()` or `Degrees()`. The internal representation is always in radians.
 *
 * Supports the full set of arithmetic and comparison operators, wrapping
 * utilities, and stream output. Angles can also be constructed from
 * user-defined literals via the `RS::Math::Literals` namespace.
 *
 * Example:
 * @code
 *     using namespace RS::Math::Literals;
 *
 *     Angle a = 90.0_deg;
 *     Angle b = Angle::Radians(1.5707f);
 *
 *     float r = a.AsRadians();   // ~1.5707
 *     float d = b.AsDegrees();   // ~90.0
 *
 *     Angle c = a + b;           // ~180°
 *     Angle w = c.WrapSigned();  // wrapped to (-180°, 180°]
 * @endcode
 */
struct Angle {

    /**
     * @brief Default constructor -- zero angle.
     *
     * Initializes the angle to 0 radians (0°).
     */
    constexpr Angle() noexcept = default;

//==========================================================================
// Factories
//==========================================================================

    /**
     * @brief Creates an angle from a value in radians.
     *
     * This is the primary factory for angles already expressed in radians.
     * No conversion is performed.
     *
     * @param radians Angle value in radians.
     * @return An `Angle` wrapping the given value.
     */
    [[nodiscard]] static constexpr Angle Radians(float radians) noexcept;

    /**
     * @brief Creates an angle from a value in degrees.
     *
     * The value is converted to radians on construction and stored internally
     * as radians. The conversion factor is `degrees * (pi / 180)`.
     *
     * @param degrees Angle value in degrees.
     * @return An `Angle` converted from degrees to radians.
     */
    [[nodiscard]] static constexpr Angle Degrees(float degrees) noexcept;

//==========================================================================
// Accessors
//==========================================================================

    /**
     * @brief Returns the angle value in radians.
     *
     * Provides direct access to the internal representation. No conversion
     * is performed.
     *
     * @return The angle in radians.
     */
    [[nodiscard]] constexpr float AsRadians() const noexcept;

    /**
     * @brief Returns the angle value in degrees.
     *
     * Converts the internal radians value to degrees using the factor
     * `radians * (180 / pi)`.
     *
     * @return The angle in degrees.
     */
    [[nodiscard]] constexpr float AsDegrees() const noexcept;

//==========================================================================
// Wrapping
//==========================================================================

    /**
     * @brief Wraps the angle to the range [-pi, pi).
     *
     * Equivalent to a signed modulo, analogous to the degree range [-180°, 180°).
     * Useful for normalizing angular differences so they always represent the
     * shortest signed arc between two orientations.
     *
     * @return A new `Angle` in [-pi, pi).
     */
    [[nodiscard]] constexpr Angle WrapSigned() const noexcept;

    /**
     * @brief Wraps the angle to the range [0, 2*pi).
     *
     * Equivalent to an unsigned modulo, analogous to the degree range [0°, 360°).
     * Useful for normalizing bearings or headings that must be non-negative.
     *
     * @return A new `Angle` in [0, 2*pi).
     */
    [[nodiscard]] constexpr Angle WrapUnsigned() const noexcept;

//==========================================================================
// Common angles
//==========================================================================

    /**
     * @brief Returns the zero angle (0 rad = 0°).
     * @return `Angle` with an internal value of 0 radians.
     */
    [[nodiscard]] static constexpr Angle Zero() noexcept;

    /**
     * @brief Returns a right angle (pi/2 rad = 90°).
     * @return `Angle` representing a quarter turn.
     */
    [[nodiscard]] static constexpr Angle Right() noexcept;

    /**
     * @brief Returns a straight angle (pi rad = 180°).
     * @return `Angle` representing a half turn.
     */
    [[nodiscard]] static constexpr Angle Straight() noexcept;

    /**
     * @brief Returns a full revolution (2*pi rad = 360°).
     * @return `Angle` representing one complete turn.
     */
    [[nodiscard]] static constexpr Angle Full() noexcept;

//==========================================================================
// Operators
//==========================================================================

    /**
     * @brief Adds another angle to this one in-place.
     * @param rhs Angle to add.
     * @return Reference to this angle after addition.
     */
    constexpr Angle& operator+=(Angle rhs) noexcept;

    /**
     * @brief Subtracts another angle from this one in-place.
     * @param rhs Angle to subtract.
     * @return Reference to this angle after subtraction.
     */
    constexpr Angle& operator-=(Angle rhs) noexcept;

    /**
     * @brief Multiplies this angle by a scalar in-place.
     * @param scalar Scalar factor.
     * @return Reference to this angle after scaling.
     */
    constexpr Angle& operator*=(float scalar) noexcept;

    /**
     * @brief Divides this angle by a scalar in-place.
     *
     * If `scalar` is zero or NaN, the angle is left unchanged.
     *
     * @param scalar Scalar divisor.
     * @return Reference to this angle after division.
     */
    constexpr Angle& operator/=(float scalar) noexcept;

    /**
     * @brief Returns the negation of this angle.
     *
     * Produces the same magnitude but in the opposite rotation direction.
     *
     * @return Negated angle.
     */
    [[nodiscard]] constexpr Angle operator-() const noexcept;

    /**
     * @brief Checks whether this angle is equal to `rhs`.
     *
     * Performs exact floating-point comparison of the internal radians values.
     * For near-equality, use a manual epsilon check on `AsRadians()`.
     *
     * @param rhs Angle to compare against.
     * @return True if both angles have identical internal values.
     */
    [[nodiscard]] constexpr bool operator==(Angle rhs) const noexcept;

    /**
     * @brief Checks whether this angle is not equal to `rhs`.
     * @param rhs Angle to compare against.
     * @return True if the internal values differ.
     */
    [[nodiscard]] constexpr bool operator!=(Angle rhs) const noexcept;

    /**
     * @brief Less-than comparison.
     * @param rhs Right-hand operand.
     * @return True if this angle is strictly less than `rhs`.
     */
    [[nodiscard]] constexpr bool operator< (Angle rhs) const noexcept;

    /**
     * @brief Greater-than comparison.
     * @param rhs Right-hand operand.
     * @return True if this angle is strictly greater than `rhs`.
     */
    [[nodiscard]] constexpr bool operator> (Angle rhs) const noexcept;

    /**
     * @brief Less-than-or-equal comparison.
     * @param rhs Right-hand operand.
     * @return True if this angle is less than or equal to `rhs`.
     */
    [[nodiscard]] constexpr bool operator<=(Angle rhs) const noexcept;

    /**
     * @brief Greater-than-or-equal comparison.
     * @param rhs Right-hand operand.
     * @return True if this angle is greater than or equal to `rhs`.
     */
    [[nodiscard]] constexpr bool operator>=(Angle rhs) const noexcept;

private:
    float m_radians{0.0f}; ///< Internal storage -- always in radians.

    /// @brief Private constructor used by `Radians()` and `Degrees()` only.
    constexpr explicit Angle(float radians) noexcept : m_radians(radians) {}
};

//==============================================================================
// Non-member arithmetic operators
//==============================================================================

/**
 * @brief Returns the sum of two angles.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return New angle equal to `lhs + rhs`.
 */
[[nodiscard]] constexpr Angle operator+(Angle lhs, Angle rhs) noexcept;

/**
 * @brief Returns the difference of two angles.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return New angle equal to `lhs - rhs`.
 */
[[nodiscard]] constexpr Angle operator-(Angle lhs, Angle rhs) noexcept;

/**
 * @brief Scales an angle by a scalar (angle on the left).
 * @param lhs Angle to scale.
 * @param scalar Scalar factor.
 * @return New angle equal to `lhs * scalar`.
 */
[[nodiscard]] constexpr Angle operator*(Angle lhs, float scalar) noexcept;

/**
 * @brief Scales an angle by a scalar (scalar on the left).
 * @param scalar Scalar factor.
 * @param rhs Angle to scale.
 * @return New angle equal to `scalar * rhs`.
 */
[[nodiscard]] constexpr Angle operator*(float scalar, Angle rhs) noexcept;

/**
 * @brief Divides an angle by a scalar.
 *
 * If `scalar` is zero or NaN, the original angle is returned unchanged.
 *
 * @param lhs Angle to divide.
 * @param scalar Scalar divisor.
 * @return New angle equal to `lhs / scalar`.
 */
[[nodiscard]] constexpr Angle operator/(Angle lhs, float scalar) noexcept;

/**
 * @brief Computes the ratio between two angles.
 *
 * Returns the dimensionless ratio `lhs.AsRadians() / rhs.AsRadians()`.
 * Useful for computing how many times one angle "fits" into another.
 *
 * @param lhs Dividend angle.
 * @param rhs Divisor angle.
 * @return Ratio of the two angles as a plain `float`.
 */
[[nodiscard]] constexpr float operator/(Angle lhs, Angle rhs) noexcept;

/**
 * @brief Writes the angle to an output stream in the format `X rad (Y°)`.
 * @param os Output stream.
 * @param a  Angle to write.
 * @return Reference to `os`.
 */
inline std::ostream& operator<<(std::ostream& os, Angle a);

//==============================================================================
// Literal suffixes
//==============================================================================

namespace Literals {

/**
 * @brief User-defined literal for degrees (floating-point).
 *
 * Allows writing angles as `90.0_deg` instead of `Angle::Degrees(90.0f)`.
 *
 * @param degrees Angle value in degrees.
 * @return `Angle` constructed from the given degree value.
 */
[[nodiscard]] constexpr Angle operator""_deg(long double degrees) noexcept;

/**
 * @brief User-defined literal for degrees (integer).
 *
 * Allows writing angles as `90_deg` instead of `Angle::Degrees(90.0f)`.
 *
 * @param degrees Angle value in degrees.
 * @return `Angle` constructed from the given degree value.
 */
[[nodiscard]] constexpr Angle operator""_deg(unsigned long long degrees) noexcept;

/**
 * @brief User-defined literal for radians (floating-point).
 *
 * Allows writing angles as `3.14_rad` instead of `Angle::Radians(3.14f)`.
 *
 * @param radians Angle value in radians.
 * @return `Angle` constructed from the given radian value.
 */
[[nodiscard]] constexpr Angle operator""_rad(long double radians) noexcept;

/**
 * @brief User-defined literal for radians (integer).
 *
 * Allows writing angles as `1_rad` instead of `Angle::Radians(1.0f)`.
 *
 * @param radians Angle value in radians.
 * @return `Angle` constructed from the given radian value.
 */
[[nodiscard]] constexpr Angle operator""_rad(unsigned long long radians) noexcept;

} // namespace Literals

} // namespace RS::Math

#include "Raysim/Math/Types/Angle.inl" // NOLINT(misc-header-include-cycle)
