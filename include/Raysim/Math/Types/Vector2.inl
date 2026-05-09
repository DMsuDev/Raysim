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

#include "Raysim/Math/Types/Vector2.hpp"   // NOLINT(misc-header-include-cycle)
#include "Raysim/Math/Utils/MathUtils.hpp"
#include "Raysim/Core/Assert.hpp"

#include <cmath>

namespace RS::Math {

//==============================================================================
// Constructors
//==============================================================================

template <typename T>
constexpr Vector2<T>::Vector2(T X, T Y) noexcept : x(X), y(Y) {}

template <typename T>
constexpr Vector2<T>::Vector2(T s) noexcept : x(s), y(s) {}

template <typename T>
template <typename U>
constexpr Vector2<T>::operator Vector2<U>() const noexcept {
    return Vector2<U>{ static_cast<U>(x), static_cast<U>(y) };
}

//==============================================================================
// Math
//==============================================================================

template <typename T>
T Vector2<T>::Length() const noexcept {
    return static_cast<T>(std::sqrt(static_cast<double>(LengthSquared())));
}

template <typename T>
constexpr T Vector2<T>::LengthSquared() const noexcept {
    return x * x + y * y;
}

template <typename T>
Vector2<T> Vector2<T>::Normalized() const noexcept {
    const T len = Length();
    if (len > T(1e-6)) return { x / len, y / len };
    return Vector2<T>{};
}

template <typename T>
Angle Vector2<T>::AngleTo(const Vector2<T>& rhs) const noexcept {
    if (IsZero() || rhs.IsZero()) return Angle::Zero();
    return Angle::Radians(std::atan2(
        static_cast<float>(Cross(rhs)),
        static_cast<float>(Dot(rhs))));
}

template <typename T>
Angle Vector2<T>::ToAngle() const noexcept {
    if (IsZero()) return Angle::Zero();
    return Angle::Radians(std::atan2(
        static_cast<float>(y),
        static_cast<float>(x)));
}

template <typename T>
Vector2<T> Vector2<T>::RotatedBy(Angle phi) const noexcept {
    const float rad = phi.AsRadians();
    const T c = static_cast<T>(std::cos(rad));
    const T s = static_cast<T>(std::sin(rad));
    return { x * c - y * s, x * s + y * c };
}

template <typename T>
constexpr Vector2<T> Vector2<T>::Project(const Vector2<T>& axis) const noexcept {
    const T d = axis.LengthSquared();
    if (d <= T(1e-12)) return Vector2<T>{};
    return axis * (Dot(axis) / d);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::Perpendicular() const noexcept {
    return { -y, x };
}

template <typename T>
constexpr T Vector2<T>::Dot(const Vector2<T>& rhs) const noexcept {
    return x * rhs.x + y * rhs.y;
}

template <typename T>
constexpr T Vector2<T>::Cross(const Vector2<T>& rhs) const noexcept {
    return x * rhs.y - y * rhs.x;
}

template <typename T>
T Vector2<T>::Distance(const Vector2<T>& rhs) const noexcept {
    return static_cast<T>(std::sqrt(static_cast<double>(DistanceSquared(rhs))));
}

template <typename T>
constexpr T Vector2<T>::DistanceSquared(const Vector2<T>& rhs) const noexcept {
    const T dx = x - rhs.x;
    const T dy = y - rhs.y;
    return dx * dx + dy * dy;
}

template <typename T>
constexpr Vector2<T> Vector2<T>::Reflect(const Vector2<T>& normal) const noexcept {
    return *this - normal * (T(2) * Dot(normal));
}

template <typename T>
constexpr Vector2<T> Vector2<T>::Rejection(const Vector2<T>& axis) const noexcept {
    return *this - Project(axis);
}

template <typename T>
void Vector2<T>::ClampLength(T maxLen) noexcept {
    const T lenSq = LengthSquared();
    if (lenSq > maxLen * maxLen) {
        const T scale = maxLen / static_cast<T>(std::sqrt(static_cast<double>(lenSq)));
        x *= scale;
        y *= scale;
    }
}

template <typename T>
void Vector2<T>::Normalize() noexcept {
    const T len = Length();
    if (len > T(1e-6)) { x /= len; y /= len; }
}

template <typename T>
void Vector2<T>::NormalizeSafe(T epsilon) noexcept {
    const T len = Length();
    if (len > epsilon) { x /= len; y /= len; return; }
    RS_CORE_WARN("Vector2::NormalizeSafe() -- vector is near-zero, zeroing out.");
    x = y = T(0);
}

template <typename T>
constexpr bool Vector2<T>::IsZero(T epsilon) const noexcept {
    const T ax = x < T(0) ? -x : x;
    const T ay = y < T(0) ? -y : y;
    return ax < epsilon && ay < epsilon;
}

//==============================================================================
// Static utilities
//==============================================================================

template <typename T>
T Vector2<T>::Distance(const Vector2<T>& a, const Vector2<T>& b) noexcept {
    return a.Distance(b);
}

template <typename T>
constexpr T Vector2<T>::DistanceSquared(const Vector2<T>& a, const Vector2<T>& b) noexcept {
    return a.DistanceSquared(b);
}

template <typename T>
constexpr Vector2<T> Vector2<T>::FromAngle(float angle) noexcept {
    return { static_cast<T>(std::cos(angle)), static_cast<T>(std::sin(angle)) };
}

template <typename T>
Angle Vector2<T>::AngleBetween(const Vector2<T>& a, const Vector2<T>& b) noexcept {
    const T mag2 = a.LengthSquared() * b.LengthSquared();
    if (mag2 > T(1e-12)) {
        const float inv = 1.0f / std::sqrt(static_cast<float>(mag2));
        const float dot = static_cast<float>(a.Dot(b)) * inv;
        return Angle::Radians(std::acos(Clamp(dot, -1.0f, 1.0f)));
    }
    return Angle::Zero();
}

//==============================================================================
// Operators
//==============================================================================

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& rhs) noexcept {
    x += rhs.x; y += rhs.y; return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& rhs) noexcept {
    x -= rhs.x; y -= rhs.y; return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator*=(T scalar) noexcept {
    x *= scalar; y *= scalar; return *this;
}

template <typename T>
constexpr Vector2<T>& Vector2<T>::operator/=(T scalar) noexcept {
    RS_CORE_ASSERT(scalar != T(0) && scalar == scalar,
                   "Vector2::operator/=: division by zero or NaN scalar.");
    *this *= (T(1) / scalar);
    return *this;
}

template <typename T>
constexpr Vector2<T> Vector2<T>::operator-() const noexcept {
    return { -x, -y };
}

template <typename T>
constexpr bool Vector2<T>::operator==(const Vector2<T>& rhs) const noexcept {
    return x == rhs.x && y == rhs.y;
}

template <typename T>
constexpr bool Vector2<T>::operator!=(const Vector2<T>& rhs) const noexcept {
    return !(*this == rhs);
}

//==============================================================================
// Common vectors
//==============================================================================

template <typename T> constexpr Vector2<T> Vector2<T>::Zero()  noexcept { return { T(0), T(0) }; }
template <typename T> constexpr Vector2<T> Vector2<T>::One()   noexcept { return { T(1), T(1) }; }
template <typename T> constexpr Vector2<T> Vector2<T>::UnitX() noexcept { return { T(1), T(0) }; }
template <typename T> constexpr Vector2<T> Vector2<T>::UnitY() noexcept { return { T(0), T(1) }; }

} // namespace RS::Math
