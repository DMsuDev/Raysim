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
 * @struct Vec3
 * @brief 3D vector with arithmetic and geometric operations.
 *
 * Represents a 3D vector for positions, velocities, and directions in 3D space.
 * Supports the standard arithmetic operators, component-wise operations, dot and
 * cross products, projection, reflection, and axis-angle rotation via
 * Rodrigues' formula.
 *
 * Example:
 * @code
 *     Vec3f a(1.0f, 0.0f, 0.0f);
 *     Vec3f b(0.0f, 1.0f, 0.0f);
 *
 *     Vec3f c   = a.Cross(b);                        // {0, 0, 1}
 *     float dot = a.Dot(b);                           // 0
 *     Vec3f r   = a.RotatedBy(Vec3f::UnitZ(), 90.0_deg); // ~{0, 1, 0}
 * @endcode
 */
template <typename T>
struct Vec3 {
    T x{0}, y{0}, z{0}; ///< Components of the vector.

    //==========================================================================
    // Constructors
    //==========================================================================

    /**
     * @brief Default constructor -- zero vector.
     *
     * Initializes all components to zero.
     */
    constexpr Vec3() noexcept = default;

    /**
     * @brief Constructs a vector with explicit x, y, z values.
     * @param X X component.
     * @param Y Y component.
     * @param Z Z component.
     */
    constexpr Vec3(T X, T Y, T Z) noexcept;

    /**
     * @brief Constructs a vector with all components set to the same value.
     * @param s Value for x, y and z.
     */
    explicit constexpr Vec3(T s) noexcept;

    /**
     * @brief Explicitly converts this vector to another Vec3 type.
     *
     * Each component is individually converted using `static_cast<U>`.
     *
     * @tparam U Target component type.
     * @return A new `Vec3<U>` with components cast to type `U`.
     */
    template <typename U>
    explicit constexpr operator Vec3<U>() const noexcept;

//==============================================================================
// Math
//==============================================================================

    /**
     * @brief Computes the Euclidean length of the vector.
     *
     * For comparisons only, prefer `LengthSquared()` to avoid the square root.
     *
     * @return Euclidean length.
     */
    [[nodiscard]] T Length() const noexcept;

    /**
     * @brief Computes the squared length of the vector.
     *
     * More efficient than `Length()` when only relative magnitudes matter.
     *
     * @return Squared length: x^2 + y^2 + z^2.
     */
    [[nodiscard]] constexpr T LengthSquared() const noexcept;

    /**
     * @brief Returns a normalized copy of this vector.
     *
     * Produces a unit vector in the same direction. Returns a zero vector
     * if the original length is effectively zero.
     *
     * @return Unit-length vector, or {0,0,0} if the vector is near-zero.
     */
    [[nodiscard]] Vec3 Normalized() const noexcept;

    /**
     * @brief Computes the dot (scalar) product with another vector.
     *
     * Measures the alignment between two vectors. Zero if perpendicular,
     * positive if pointing in a similar direction, negative if opposite.
     *
     * @param rhs Right-hand operand.
     * @return Scalar dot product: x*rhs.x + y*rhs.y + z*rhs.z.
     */
    [[nodiscard]] constexpr T Dot(const Vec3& rhs) const noexcept;

    /**
     * @brief Computes the 3D cross product with another vector.
     *
     * Returns a vector perpendicular to both `*this` and `rhs`, following the
     * right-hand rule. The magnitude equals the area of the parallelogram
     * formed by the two vectors.
     *
     * @param rhs Right-hand operand.
     * @return Cross product vector: this x rhs.
     */
    [[nodiscard]] constexpr Vec3 Cross(const Vec3& rhs) const noexcept;

    /**
     * @brief Projects this vector onto the given axis.
     *
     * Computes the component of this vector along `axis`. If `axis` has zero
     * length, returns a zero vector.
     *
     * Mathematically: proj = (Dot(axis) / axis.LengthSquared()) * axis
     *
     * @param axis Projection axis.
     * @return Projection of this vector onto `axis`.
     */
    [[nodiscard]] constexpr Vec3 Project(const Vec3& axis) const noexcept;

    /**
     * @brief Computes the rejection of this vector from the given axis.
     *
     * The component of this vector perpendicular to `axis`:
     *     rejection = this - projection
     *
     * @param axis Projection axis.
     * @return Component of this vector perpendicular to `axis`.
     */
    [[nodiscard]] constexpr Vec3 Rejection(const Vec3& axis) const noexcept;

    /**
     * @brief Returns this vector reflected across a surface normal.
     *
     * The normal should be normalized. Reflection formula:
     *     reflected = v - 2 * Dot(v, normal) * normal
     *
     * @param normal Normalized surface normal.
     * @return Reflected vector.
     */
    [[nodiscard]] constexpr Vec3 Reflect(const Vec3& normal) const noexcept;

    /**
     * @brief Rotates this vector around an arbitrary axis using Rodrigues' formula.
     *
     * The axis does not need to be pre-normalized -- it is normalized internally.
     * The rotation follows the right-hand rule.
     *
     * @param axis  Rotation axis (will be normalized).
     * @param angle Rotation angle.
     * @return Rotated vector.
     */
    [[nodiscard]] Vec3 RotatedBy(const Vec3& axis, Angle angle) const noexcept;

    /**
     * @brief Computes the Euclidean distance from this vector to `rhs`.
     *
     * Equivalent to `(*this - rhs).Length()`. For comparisons only, prefer
     * `DistanceSquared()`.
     *
     * @param rhs Other point.
     * @return Euclidean distance.
     */
    [[nodiscard]] T Distance(const Vec3& rhs) const noexcept;

    /**
     * @brief Computes the squared distance from this vector to `rhs`.
     *
     * More efficient than `Distance()` -- avoids the square root.
     *
     * @param rhs Other point.
     * @return Squared Euclidean distance.
     */
    [[nodiscard]] constexpr T DistanceSquared(const Vec3& rhs) const noexcept;

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
     * If the vector is near-zero, the operation is skipped and a warning is logged.
     */
    void Normalize() noexcept;

    /**
     * @brief Safely normalizes this vector in-place.
     *
     * If the vector is near-zero, it is set to {0,0,0} to avoid NaN propagation.
     *
     * @param epsilon Threshold below which the vector is considered zero.
     */
    void NormalizeSafe(T epsilon = T(1e-6)) noexcept;

    /**
     * @brief Checks whether the vector is effectively zero.
     *
     * Uses an epsilon threshold instead of exact comparison.
     *
     * @param epsilon Tolerance for zero comparison.
     * @return True if all components are below epsilon in absolute value.
     */
    [[nodiscard]] constexpr bool IsZero(T epsilon = T(1e-6)) const noexcept;

//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Computes the Euclidean distance between two points.
     * @param a First point.
     * @param b Second point.
     * @return Euclidean distance between a and b.
     */
    [[nodiscard]] static T Distance(const Vec3& a, const Vec3& b) noexcept;

    /**
     * @brief Computes the squared Euclidean distance between two points.
     * @param a First point.
     * @param b Second point.
     * @return Squared distance between a and b.
     */
    [[nodiscard]] static constexpr T DistanceSquared(const Vec3& a, const Vec3& b) noexcept;

    /**
     * @brief Creates a unit vector from an angle in the XY plane.
     *
     * The angle is measured from the positive x-axis. The z component is 0.
     *
     * @param angle Angle in the XY plane.
     * @return Unit vector on the XY plane.
     */
    [[nodiscard]] static Vec3 FromAngleXY(Angle angle) noexcept;

    /**
     * @brief Creates a unit vector from an angle in the XZ plane.
     *
     * The angle is measured from the positive x-axis. The y component is 0.
     *
     * @param angle Angle in the XZ plane.
     * @return Unit vector on the XZ plane.
     */
    [[nodiscard]] static Vec3 FromAngleXZ(Angle angle) noexcept;

    /**
     * @brief Creates a vector from spherical coordinates.
     *
     * Uses the physics convention: polar is the inclination from the +y axis,
     * azimuth is the rotation around y in the XZ plane.
     *
     *     x = radius * sin(polar) * cos(azimuth)
     *     y = radius * cos(polar)
     *     z = radius * sin(polar) * sin(azimuth)
     *
     * @param polar   Inclination angle from the +y axis (0 = up, pi = down).
     * @param azimuth Rotation angle around the y axis.
     * @param radius  Length of the resulting vector (default 1).
     * @return Vector in Cartesian coordinates.
     */
    [[nodiscard]] static Vec3 FromSpherical(Angle polar, Angle azimuth, T radius = T(1)) noexcept;

    /**
     * @brief Computes the unsigned angle between two vectors.
     *
     * Result is always in [0°, 180°]. If either vector has zero length,
     * returns `Angle::Zero()`. Computed as acos(Dot / (|a| * |b|)).
     *
     * @param a First vector.
     * @param b Second vector.
     * @return Unsigned angle between a and b.
     */
    [[nodiscard]] static Angle AngleBetween(const Vec3& a, const Vec3& b) noexcept;

//==============================================================================
// Operators
//==============================================================================

    /**
     * @brief Adds `rhs` to this vector in-place.
     * @param rhs Vector to add.
     * @return Reference to this vector after addition.
     */
    constexpr Vec3& operator+=(const Vec3& rhs) noexcept;

    /**
     * @brief Subtracts `rhs` from this vector in-place.
     * @param rhs Vector to subtract.
     * @return Reference to this vector after subtraction.
     */
    constexpr Vec3& operator-=(const Vec3& rhs) noexcept;

    /**
     * @brief Multiplies this vector by a scalar in-place.
     * @param scalar Scalar factor.
     * @return Reference to this vector after scaling.
     */
    constexpr Vec3& operator*=(T scalar) noexcept;

    /**
     * @brief Divides this vector by a scalar in-place.
     *
     * If `scalar` is zero or NaN, the vector is left unchanged and a
     * debug assertion is triggered.
     *
     * @param scalar Scalar divisor.
     * @return Reference to this vector after division.
     */
    constexpr Vec3& operator/=(T scalar) noexcept;

    /**
     * @brief Returns the negation of this vector.
     * @return Vector with all components negated.
     */
    [[nodiscard]] constexpr Vec3 operator-() const noexcept;

    /**
     * @brief Checks whether this vector is equal to `rhs`.
     *
     * Performs exact component-wise comparison. For floating-point types,
     * consider an epsilon-based comparison when appropriate.
     *
     * @param rhs Vector to compare against.
     * @return True if all components are equal.
     */
    [[nodiscard]] constexpr bool operator==(const Vec3& rhs) const noexcept;

    /**
     * @brief Checks whether this vector is not equal to `rhs`.
     * @param rhs Vector to compare against.
     * @return True if any component differs.
     */
    [[nodiscard]] constexpr bool operator!=(const Vec3& rhs) const noexcept;

    /**
     * @brief Accesses a component by index (0 = x, 1 = y, 2 = z).
     *
     * No bounds checking is performed.
     *
     * @param i Component index (0, 1 or 2).
     * @return Reference to the component.
     */
    constexpr T&       operator[](size_t i)       noexcept { return (&x)[i]; }
    /// @copydoc operator[](size_t)
    constexpr const T& operator[](size_t i) const noexcept { return (&x)[i]; }

//==============================================================================
// Common vectors
//==============================================================================

    /**
     * @brief Returns the zero vector {0, 0, 0}.
     * @return Vec3 with all components set to zero.
     */
    [[nodiscard]] static constexpr Vec3 Zero()  noexcept;

    /**
     * @brief Returns the one vector {1, 1, 1}.
     * @return Vec3 with all components set to one.
     */
    [[nodiscard]] static constexpr Vec3 One()   noexcept;

    /**
     * @brief Returns the unit vector along the x-axis {1, 0, 0}.
     * @return Vec3 pointing in the positive x direction.
     */
    [[nodiscard]] static constexpr Vec3 UnitX() noexcept;

    /**
     * @brief Returns the unit vector along the y-axis {0, 1, 0}.
     * @return Vec3 pointing in the positive y direction.
     */
    [[nodiscard]] static constexpr Vec3 UnitY() noexcept;

    /**
     * @brief Returns the unit vector along the z-axis {0, 0, 1}.
     * @return Vec3 pointing in the positive z direction.
     */
    [[nodiscard]] static constexpr Vec3 UnitZ() noexcept;
};

// Common type aliases
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3u = Vec3<unsigned int>;

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
[[nodiscard]] constexpr Vec3<T> operator+(Vec3<T> lhs, const Vec3<T>& rhs) noexcept { lhs += rhs; return lhs; }

/**
 * @brief Component-wise subtraction of two vectors.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return New vector equal to `lhs - rhs`.
 */
template <typename T>
[[nodiscard]] constexpr Vec3<T> operator-(Vec3<T> lhs, const Vec3<T>& rhs) noexcept { lhs -= rhs; return lhs; }

/**
 * @brief Multiplies a vector by a scalar (vector on the left).
 * @param lhs Vector to scale.
 * @param scalar Scalar factor.
 * @return New scaled vector.
 */
template <typename T>
[[nodiscard]] constexpr Vec3<T> operator*(Vec3<T> lhs, T scalar) noexcept { lhs *= scalar; return lhs; }

/**
 * @brief Multiplies a scalar by a vector (scalar on the left).
 * @param scalar Scalar factor.
 * @param rhs Vector to scale.
 * @return New scaled vector.
 */
template <typename T>
[[nodiscard]] constexpr Vec3<T> operator*(T scalar, Vec3<T> rhs) noexcept { rhs *= scalar; return rhs; }

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
[[nodiscard]] constexpr Vec3<T> operator/(Vec3<T> lhs, T scalar) noexcept { lhs /= scalar; return lhs; }

/**
 * @brief Writes the vector to an output stream in the format `(x, y, z)`.
 * @param os Output stream.
 * @param v  Vector to write.
 * @return Reference to `os`.
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace RS::Math

#include "Raysim/Math/Types/Vec3.inl" // NOLINT(misc-header-include-cycle)
