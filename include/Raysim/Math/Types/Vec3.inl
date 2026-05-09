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

// This file is included at the bottom of Vec3.hpp -- do not include it directly.

#pragma once

#include "Raysim/Core/Assert.hpp"
#include "Raysim/Math/Types/Angle.hpp"
#include "Raysim/Math/Utils/MathUtils.hpp"

#include <cmath>

namespace RS::Math {

//==============================================================================
// Constructors
//==============================================================================

template <typename T>
constexpr Vec3<T>::Vec3(T X, T Y, T Z) noexcept
    : x(X), y(Y), z(Z) {}

template <typename T>
constexpr Vec3<T>::Vec3(T s) noexcept
    : x(s), y(s), z(s) {}

template <typename T>
template <typename U>
constexpr Vec3<T>::operator Vec3<U>() const noexcept {
    return Vec3<U>{
        static_cast<U>(x),
        static_cast<U>(y),
        static_cast<U>(z)
    };
}

//==============================================================================
// Math
//==============================================================================

template <typename T>
T Vec3<T>::Length() const noexcept {
    return static_cast<T>(std::sqrt(static_cast<double>(LengthSquared())));
}

template <typename T>
constexpr T Vec3<T>::LengthSquared() const noexcept {
    return x*x + y*y + z*z;
}

template <typename T>
Vec3<T> Vec3<T>::Normalized() const noexcept {
    const T len = Length();
    if (len > T(1e-6)) return Vec3<T>{x / len, y / len, z / len};
    return Vec3<T>{};
}

template <typename T>
constexpr T Vec3<T>::Dot(const Vec3& rhs) const noexcept {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

template <typename T>
constexpr Vec3<T> Vec3<T>::Cross(const Vec3& rhs) const noexcept {
    return Vec3<T>{
        y * rhs.z - z * rhs.y,
        z * rhs.x - x * rhs.z,
        x * rhs.y - y * rhs.x
    };
}

template <typename T>
constexpr Vec3<T> Vec3<T>::Project(const Vec3& axis) const noexcept {
    const T denom = axis.LengthSquared();
    if (denom > T(1e-12)) return axis * (Dot(axis) / denom);
    return Vec3<T>{};
}

template <typename T>
constexpr Vec3<T> Vec3<T>::Rejection(const Vec3& axis) const noexcept {
    return *this - Project(axis);
}

template <typename T>
constexpr Vec3<T> Vec3<T>::Reflect(const Vec3& normal) const noexcept {
    return *this - normal * (T(2) * Dot(normal));
}

template <typename T>
Vec3<T> Vec3<T>::RotatedBy(const Vec3& axis, Angle angle) const noexcept {
    const Vec3<T> k = axis.Normalized();
    const float rad = angle.AsRadians();
    const float c   = std::cos(rad);
    const float s   = std::sin(rad);

    return *this * static_cast<T>(c)
         + k.Cross(*this) * static_cast<T>(s)
         + k * static_cast<T>(k.Dot(*this) * (1.0f - c));
}

template <typename T>
T Vec3<T>::Distance(const Vec3& rhs) const noexcept {
    return (*this - rhs).Length();
}

template <typename T>
constexpr T Vec3<T>::DistanceSquared(const Vec3& rhs) const noexcept {
    return (*this - rhs).LengthSquared();
}

template <typename T>
void Vec3<T>::ClampLength(T maxLen) noexcept {
    const T lenSq = LengthSquared();
    if (lenSq > maxLen * maxLen) {
        const T scale = maxLen / static_cast<T>(std::sqrt(static_cast<double>(lenSq)));
        x *= scale;
        y *= scale;
        z *= scale;
    }
}

template <typename T>
void Vec3<T>::Normalize() noexcept {
    const T len = Length();
    if (len > T(1e-6)) { *this /= len; return; }
    RS_CORE_WARN("Vec3::Normalize() called on a near-zero vector -- skipping.");
}

template <typename T>
void Vec3<T>::NormalizeSafe(T epsilon) noexcept {
    const T len = Length();
    if (len > epsilon) { *this /= len; return; }
    RS_CORE_WARN("Vec3::NormalizeSafe() -- near-zero vector set to zero.");
    x = y = z = T(0);
}

template <typename T>
constexpr bool Vec3<T>::IsZero(T epsilon) const noexcept {
    return std::abs(x) < epsilon
        && std::abs(y) < epsilon
        && std::abs(z) < epsilon;
}

//==============================================================================
// Static utilities
//==============================================================================

template <typename T>
T Vec3<T>::Distance(const Vec3& a, const Vec3& b) noexcept {
    return (b - a).Length();
}

template <typename T>
constexpr T Vec3<T>::DistanceSquared(const Vec3& a, const Vec3& b) noexcept {
    return (b - a).LengthSquared();
}

template <typename T>
Vec3<T> Vec3<T>::FromAngleXY(Angle angle) noexcept {
    const float rad = angle.AsRadians();
    return Vec3<T>{static_cast<T>(std::cos(rad)), static_cast<T>(std::sin(rad)), T(0)};
}

template <typename T>
Vec3<T> Vec3<T>::FromAngleXZ(Angle angle) noexcept {
    const float rad = angle.AsRadians();
    return Vec3<T>{static_cast<T>(std::cos(rad)), T(0), static_cast<T>(std::sin(rad))};
}

template <typename T>
Vec3<T> Vec3<T>::FromSpherical(Angle polar, Angle azimuth, T radius) noexcept {
    const float p = polar.AsRadians();
    const float a = azimuth.AsRadians();
    const float sp = std::sin(p);
    return Vec3<T>{
        static_cast<T>(radius) * static_cast<T>(sp * std::cos(a)),
        static_cast<T>(radius) * static_cast<T>(std::cos(p)),
        static_cast<T>(radius) * static_cast<T>(sp * std::sin(a))
    };
}

template <typename T>
Angle Vec3<T>::AngleBetween(const Vec3& a, const Vec3& b) noexcept {
    const T la2 = a.LengthSquared();
    const T lb2 = b.LengthSquared();
    if (la2 > T(1e-12) && lb2 > T(1e-12)) {
        const T inv  = T(1) / static_cast<T>(std::sqrt(static_cast<double>(la2) * static_cast<double>(lb2)));
        const float dot = static_cast<float>(a.Dot(b) * inv);
        return Angle::Radians(std::acos(Clamp(dot, -1.0f, 1.0f)));
    }
    return Angle::Zero();
}

//==============================================================================
// Operators
//==============================================================================

template <typename T>
constexpr Vec3<T>& Vec3<T>::operator+=(const Vec3& rhs) noexcept {
    x += rhs.x; y += rhs.y; z += rhs.z;
    return *this;
}

template <typename T>
constexpr Vec3<T>& Vec3<T>::operator-=(const Vec3& rhs) noexcept {
    x -= rhs.x; y -= rhs.y; z -= rhs.z;
    return *this;
}

template <typename T>
constexpr Vec3<T>& Vec3<T>::operator*=(T scalar) noexcept {
    x *= scalar; y *= scalar; z *= scalar;
    return *this;
}

template <typename T>
constexpr Vec3<T>& Vec3<T>::operator/=(T scalar) noexcept {
    RS_CORE_ASSERT(scalar != T(0) && scalar == scalar,
                   "Vec3::operator/=: division by zero or NaN scalar.");
    x /= scalar; y /= scalar; z /= scalar;
    return *this;
}

template <typename T>
constexpr Vec3<T> Vec3<T>::operator-() const noexcept {
    return Vec3<T>{-x, -y, -z};
}

template <typename T>
constexpr bool Vec3<T>::operator==(const Vec3& rhs) const noexcept {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

template <typename T>
constexpr bool Vec3<T>::operator!=(const Vec3& rhs) const noexcept {
    return !(*this == rhs);
}

//==============================================================================
// Common vectors
//==============================================================================

template <typename T>
constexpr Vec3<T> Vec3<T>::Zero() noexcept {
    return Vec3<T>{T(0), T(0), T(0)};
}

template <typename T>
constexpr Vec3<T> Vec3<T>::One() noexcept {
    return Vec3<T>{T(1), T(1), T(1)};
}

template <typename T>
constexpr Vec3<T> Vec3<T>::UnitX() noexcept {
    return Vec3<T>{T(1), T(0), T(0)};
}

template <typename T>
constexpr Vec3<T> Vec3<T>::UnitY() noexcept {
    return Vec3<T>{T(0), T(1), T(0)};
}

template <typename T>
constexpr Vec3<T> Vec3<T>::UnitZ() noexcept {
    return Vec3<T>{T(0), T(0), T(1)};
}

} // namespace RS::Math
