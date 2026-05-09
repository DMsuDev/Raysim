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

#include "Raysim/Math/Types/Angle.hpp"

#include <cstdint>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vector2
 * @brief 2D vector with arithmetic and geometric operations
 */
template <typename T>
struct Vector2 {
    // Components of the vector
    T x{0}; ///< X component of the vector
    T y{0}; ///< Y component of the vector

    /**
     * @brief Default constructor (zero vector)
     */
    constexpr Vector2() noexcept = default;
    /**
     * @brief Constructor with explicit x and y values
     * @param X X component
     * @param Y Y component
     */
    constexpr Vector2(T X, T Y) noexcept;
    /**
     * @brief Constructor that sets both components to the same value
     * @param s Value for both x and y
     */
    explicit constexpr Vector2(T s) noexcept;

    /**
     * @brief Explicitly converts this vector to another Vector2 type.
     *
     * Enables explicit conversion from `Vector2<T>` to `Vector2<U>`, allowing safe and
     * intentional casting between different numeric types (e.g., float -> int,
     * double -> float). Each component is individually converted using
     * `static_cast<U>`.
     *
     * This is useful when interacting with APIs or subsystems that require a
     * specific numeric type.
     *
     * Example:
     *
     *     Vector2<float> vf(1.5f, 2.8f);
     *     Vector2<int>   vi = static_cast<Vector2<int>>(vf);
     *     // vi == {1, 2}
     *
     * @tparam U Target component type (e.g., int, float, double).
     * @return A new `Vector2<U>` with both components converted to type `U`.
     */
    template <typename U>
    explicit constexpr operator Vector2<U>() const noexcept;

//==============================================================================
// Math
//==============================================================================

    /**
     * @brief Calculate vector magnitude (length)
     *
     * If you are not interested in the actual length, but only in comparisons, consider using `LengthSquared()`.
     *
     * @return Euclidean length of the vector
     */
    [[nodiscard]] T Length() const noexcept;

    /**
     * @brief Calculate squared magnitude (faster, avoids sqrt)
     *
     * For performance-sensitive code where you only need to compare lengths,
     * using `LengthSquared()` is more efficient than `Length()`, as it avoids the costly square root operation.
     *
     * @return Squared length of the vector
     */
    [[nodiscard]] constexpr T LengthSquared() const noexcept;

    /**
     * @brief Returns a normalized copy of the vector.
     *
     * Produces a vector with the same direction but a length of 1.
     * If the vector has zero length, a zero vector is returned.
     *
     * @return Unit-length vector in the same direction, or {0,0} if the vector is zero.
     */
    [[nodiscard]] Vector2 Normalized() const noexcept;

    /**
     * @brief Computes the signed angle from this vector to another.
     *
     * Returns the smallest signed angular difference between this vector and `rhs`.
     * The angle is measured in radians, positive for counter‑clockwise rotation and
     * negative for clockwise rotation.
     *
     * If either vector has zero length, the returned angle is 0.
     *
     * @param rhs Target vector to measure the angle to.
     * @return Signed angle in radians from this vector to `rhs`.
     */
    [[nodiscard]] Angle AngleTo(const Vector2<T>& rhs) const noexcept;

    /**
     * @brief Returns the angle of this vector from the positive x-axis.
     *
     * The angle is measured in radians, in the range (-pi, pi].
     * If the vector has zero length, the returned angle is 0.
     *
     * @return Angle in radians from the positive x-axis to this vector.
     */
    [[nodiscard]] Angle ToAngle() const noexcept;

    /**
     * @brief Returns a new vector obtained by rotating this one by the given angle.
     *
     * The rotation is performed around the origin using the standard 2D rotation
     * matrix:
     *
     *     x' = x * cos(phi) - y * sin(phi)
     *     y' = x * sin(phi) + y * cos(phi)
     *
     * The angle is expressed in radians. Positive values rotate the vector
     * counter‑clockwise, negative values clockwise. The vector's magnitude is
     * preserved.
     *
     * @param phi Rotation angle in radians.
     * @return Rotated vector.
     */
    [[nodiscard]] Vector2 RotatedBy(Angle phi) const noexcept;

    /**
     * @brief Projects this vector onto the given axis.
     *
     * Computes the vector projection of this vector onto `axis`. The result is the
     * component of this vector that lies in the direction of `axis`. If `axis` has
     * zero length, the returned vector is {0,0}.
     *
     * Mathematically:
     *     proj_axis(this) = (Dot(axis) / axis.LengthSquared()) * axis
     *
     * @param axis Vector defining the projection axis.
     * @return Projection of this vector onto `axis`, or {0,0} if `axis` is zero.
     */
    [[nodiscard]] constexpr Vector2 Project(const Vector2& axis) const noexcept;

    /**
     * @brief Returns a vector perpendicular to this one.
     *
     * Computes a 90‑degree counter‑clockwise rotation of the vector. The resulting
     * vector has the same magnitude but is orthogonal to the original. For a vector
     * (x, y), the perpendicular vector is (-y, x).
     *
     * @return A vector perpendicular to this one, rotated 90° counter‑clockwise.
     */
    [[nodiscard]] constexpr Vector2 Perpendicular() const noexcept;

    /**
     * @brief Computes the dot (scalar) product with another vector.
     *
     * The dot product measures how aligned two vectors are. It is positive when
     * both vectors point in a similar direction, negative when they point in
     * opposite directions, and zero when they are perpendicular.
     *
     * @param rhs Vector to compute the dot product with.
     * @return Scalar dot product: x * rhs.x + y * rhs.y.
     */
    [[nodiscard]] constexpr T Dot(const Vector2& rhs) const noexcept;

    /**
     * @brief Computes the 2D cross product (perpendicular magnitude) with another vector.
     *
     * In 2D, the cross product is a scalar representing the signed magnitude of the
     * perpendicular vector that would result from a 3D cross product. A positive
     * value indicates that `rhs` is to the left (counter‑clockwise) of this vector,
     * and a negative value indicates it is to the right (clockwise).
     *
     * @param rhs Vector to compute the cross product with.
     * @return Signed scalar cross product: x * rhs.y - y * rhs.x.
     */
    [[nodiscard]] constexpr T Cross(const Vector2& rhs) const noexcept;

    /**
     * @brief Computes the Euclidean distance between this vector and `rhs`.
     *
     * Equivalent to the length of (this - rhs). Useful for measuring the distance
     * between two points in 2D space. If you only need to compare distances,
     * consider using DistanceSquared() for better performance.
     *
     * @param rhs Other vector (point) to measure distance to.
     * @return Euclidean distance between the two vectors.
     */
    [[nodiscard]] T Distance(const Vector2& rhs) const noexcept;

    /**
     * @brief Computes the squared distance between this vector and `rhs`.
     *
     * More efficient than Distance(), as it avoids the square root. Suitable for
     * performance‑critical code where only relative distances matter.
     *
     * @param rhs Other vector (point) to measure squared distance to.
     * @return Squared Euclidean distance between the two vectors.
     */
    [[nodiscard]] constexpr T DistanceSquared(const Vector2& rhs) const noexcept;

    /**
     * @brief Returns this vector reflected across a given normal.
     *
     * The normal vector must be normalized. The reflection is computed as:
     *
     *     reflected = v - 2 * Dot(v, normal) * normal
     *
     * Commonly used in physics (bounces, collisions) and lighting calculations.
     *
     * @param normal Normalized vector defining the reflection plane.
     * @return Reflected vector.
     */
    [[nodiscard]] constexpr Vector2 Reflect(const Vector2& normal) const noexcept;

    /**
     * @brief Computes the rejection of this vector from the given axis.
     *
     * The rejection is the component of this vector perpendicular to `axis`.
     * Mathematically:
     *
     *     rejection = this - projection
     *
     * Useful for decomposing motion, collision resolution, and separating axes.
     *
     * @param axis Vector defining the projection axis.
     * @return Component of this vector perpendicular to `axis`.
     */
    [[nodiscard]] constexpr Vector2 Rejection(const Vector2& axis) const noexcept;

    /**
     * @brief Clamps the vector's magnitude to a maximum value.
     *
     * If the vector's length exceeds `maxLen`, it is scaled down to match it.
     * If the vector is zero, it remains unchanged.
     *
     * @param maxLen Maximum allowed length.
     */
    void ClampLength(T maxLen) noexcept;

    /**
     * @brief Normalizes this vector in-place.
     *
     * If the vector has non-zero length, it is scaled to unit length. If the
     * magnitude is extremely small, the vector is left unchanged to avoid
     * numerical instability.
     */
    void Normalize() noexcept;

    /**
     * @brief Safely normalizes this vector in-place.
     *
     * If the vector has non-zero length, it becomes a unit vector. If the vector
     * is too close to zero length, it becomes {0,0} to avoid division by zero
     * and NaN propagation.
     *
     * @param epsilon Threshold below which the vector is considered zero.
     */
    void NormalizeSafe(T epsilon = T(1e-6)) noexcept;

    /**
     * @brief Checks whether the vector is effectively zero.
     *
     * Useful for avoiding division by zero, normalization errors, and unstable
     * calculations. Uses an epsilon threshold instead of exact comparison.
     *
     * @param epsilon Tolerance for zero comparison.
     * @return True if |x| and |y| are both below epsilon.
     */
    [[nodiscard]] constexpr bool IsZero(T epsilon = T(1e-6)) const noexcept;


//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Computes the Euclidean distance between two points.
     *
     * Equivalent to (b - a).Length(). Provided for convenience when neither vector
     * is conceptually the “owner” of the operation. If you only need to compare
     * distances, consider using DistanceSquared() for better performance.
     *
     * @param a First point.
     * @param b Second point.
     * @return Euclidean distance between a and b.
     */
    [[nodiscard]] static T Distance(const Vector2& a, const Vector2& b) noexcept;

    /**
     * @brief Computes the squared Euclidean distance between two points.
     *
     * More efficient than Distance(), as it avoids the square root. Suitable for
     * performance‑critical code where only relative distances matter.
     *
     * @param a First point.
     * @param b Second point.
     * @return Squared distance between a and b.
     */
    [[nodiscard]] static constexpr T DistanceSquared(const Vector2& a, const Vector2& b) noexcept;

    /**
     * @brief Creates a unit vector from an angle in radians.
     *
     * The angle is measured from the positive x-axis, counter‑clockwise. The
     * resulting vector has length 1.
     *
     * @param angle Angle in radians.
     * @return Unit vector pointing in the direction of the given angle.
     */
    [[nodiscard]] static constexpr Vector2 FromAngle(float angle) noexcept;

    /**
     * @brief Computes the unsigned angle between two vectors.
     *
     * The result is always in the range [0, pi]. If either vector has zero length,
     * the returned angle is 0. Computed using the normalized dot product:
     *
     *     angle = acos( Dot(a, b) / (|a| * |b|) )
     *
     * @param a First vector.
     * @param b Second vector.
     * @return Unsigned angle between a and b.
     */
    [[nodiscard]] static Angle AngleBetween(const Vector2& a, const Vector2& b) noexcept;

//==============================================================================
// Operators
//==============================================================================

    /**
     * @brief Adds `rhs` to this vector in-place.
     * @param rhs Vector to add.
     * @return Reference to this vector after addition.
     */
    constexpr Vector2& operator+=(const Vector2& rhs) noexcept;

    /**
     * @brief Subtracts `rhs` from this vector in-place.
     * @param rhs Vector to subtract.
     * @return Reference to this vector after subtraction.
     */
    constexpr Vector2& operator-=(const Vector2& rhs) noexcept;

    /**
     * @brief Multiplies this vector by a scalar in-place.
     * @param scalar Scalar factor.
     * @return Reference to this vector after scaling.
     */
    constexpr Vector2& operator*=(T scalar) noexcept;

    /**
     * @brief Divides this vector by a scalar in-place.
     *
     * If `scalar` is zero or NaN, the vector is left unchanged.
     *
     * @param scalar Scalar divisor.
     * @return Reference to this vector after division.
     */
    constexpr Vector2& operator/=(T scalar) noexcept;

    /**
     * @brief Returns the negation of this vector.
     * @return Vector with both components negated.
     */
    [[nodiscard]] constexpr Vector2 operator-() const noexcept;

    /**
     * @brief Checks whether this vector is equal to `rhs`.
     *
     * Performs exact component-wise comparison. For floating-point types consider
     * an epsilon-based comparison when appropriate.
     *
     * @param rhs Vector to compare against.
     * @return True if both components are equal.
     */
    [[nodiscard]] constexpr bool operator==(const Vector2& rhs) const noexcept;

    /**
     * @brief Checks whether this vector is not equal to `rhs`.
     * @param rhs Vector to compare against.
     * @return True if any component differs.
     */
    [[nodiscard]] constexpr bool operator!=(const Vector2& rhs) const noexcept;

    /**
     * @brief Accesses a component by index (0 = x, 1 = y).
     *
     * No bounds checking is performed.
     *
     * @param i Component index (0 or 1).
     * @return Reference to the component.
     */
    constexpr T&       operator[](size_t i)       noexcept { return (&x)[i]; }
    /// @copydoc operator[](size_t)
    constexpr const T& operator[](size_t i) const noexcept { return (&x)[i]; }

//==============================================================================
// Common vectors
//==============================================================================

    /**
     * @brief Returns the zero vector {0, 0}.
     * @return Vector2 with both components set to zero.
     */
    [[nodiscard]] static constexpr Vector2 Zero()  noexcept;

    /**
     * @brief Returns the one vector {1, 1}.
     * @return Vector2 with both components set to one.
     */
    [[nodiscard]] static constexpr Vector2 One()   noexcept;

    /**
     * @brief Returns the unit vector along the x-axis {1, 0}.
     * @return Vector2 pointing in the positive x direction.
     */
    [[nodiscard]] static constexpr Vector2 UnitX() noexcept;

    /**
     * @brief Returns the unit vector along the y-axis {0, 1}.
     * @return Vector2 pointing in the positive y direction.
     */
    [[nodiscard]] static constexpr Vector2 UnitY() noexcept;
};

// Define the most common types
using Vec2i = Vector2<int>;
using Vec2f = Vector2<float>;
using Vec2d = Vector2<double>;
using Vec2u = Vector2<unsigned int>;
using Vec2u16 = Vector2<std::uint16_t>;
using Vec2u32 = Vector2<std::uint32_t>;

//==============================================================================
// Non-member arithmetic operators
//==============================================================================

/**
 * @brief Component-wise addition of two vectors.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return New vector equal to `lhs + rhs`.
 */
template <typename T>
[[nodiscard]] constexpr Vector2<T> operator+(Vector2<T> lhs, const Vector2<T>& rhs) noexcept { lhs += rhs; return lhs; }

/**
 * @brief Component-wise subtraction of two vectors.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return New vector equal to `lhs - rhs`.
 */
template <typename T>
[[nodiscard]] constexpr Vector2<T> operator-(Vector2<T> lhs, const Vector2<T>& rhs) noexcept { lhs -= rhs; return lhs; }

/**
 * @brief Multiplies a vector by a scalar (vector on the left).
 * @param lhs Vector to scale.
 * @param scalar Scalar factor.
 * @return New scaled vector.
 */
template <typename T>
[[nodiscard]] constexpr Vector2<T> operator*(Vector2<T> lhs, T scalar) noexcept { lhs *= scalar; return lhs; }

/**
 * @brief Multiplies a scalar by a vector (scalar on the left).
 * @param scalar Scalar factor.
 * @param rhs Vector to scale.
 * @return New scaled vector.
 */
template <typename T>
[[nodiscard]] constexpr Vector2<T> operator*(T scalar, Vector2<T> rhs) noexcept { rhs *= scalar; return rhs; }

/**
 * @brief Divides a vector by a scalar.
 *
 * If `scalar` is zero or NaN, the original vector is returned unchanged.
 *
 * @param lhs Vector to divide.
 * @param scalar Scalar divisor.
 * @return New divided vector.
 */
template <typename T>
[[nodiscard]] constexpr Vector2<T> operator/(Vector2<T> lhs, T scalar) noexcept { lhs /= scalar; return lhs; }

/**
 * @brief Writes the vector to an output stream in the format `(x, y)`.
 * @param os Output stream.
 * @param v Vector to write.
 * @return Reference to `os`.
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

} // namespace RS::Math

#include "Raysim/Math/Types/Vector2.inl"
