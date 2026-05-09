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

#include "Raysim/Math/Types/Angle.hpp"     // NOLINT(misc-header-include-cycle)
#include "Raysim/Math/Utils/MathUtils.hpp"

#include <cmath>

namespace RS::Math {

//==============================================================================
// Construction
//==============================================================================

constexpr Angle Angle::Radians(float radians) noexcept {
    return Angle{ radians };
}

constexpr Angle Angle::Degrees(float degrees) noexcept {
    return Angle{ degrees * DEG2RAD };
}

//==============================================================================
// Accessors
//==============================================================================

constexpr float Angle::AsRadians() const noexcept {
    return m_radians;
}

constexpr float Angle::AsDegrees() const noexcept {
    return m_radians * RAD2DEG;
}

//==============================================================================
// Wrapping
//==============================================================================

constexpr Angle Angle::WrapSigned() const noexcept {
    // Wrap to [-pi, pi) using a constexpr-friendly fmod approximation.
    // For runtime paths std::fmod is fine; for constexpr we rely on the fact
    // that most compilers evaluate it at compile time for constant expressions.
    const float wrapped = m_radians - TWO_PI * static_cast<int>(m_radians / TWO_PI);
    if (wrapped >= PI)  return Angle{ wrapped - TWO_PI };
    if (wrapped < -PI)  return Angle{ wrapped + TWO_PI };
    return Angle{ wrapped };
}

constexpr Angle Angle::WrapUnsigned() const noexcept {
    const float wrapped = m_radians - TWO_PI * static_cast<int>(m_radians / TWO_PI);
    if (wrapped < 0.0f) return Angle{ wrapped + TWO_PI };
    return Angle{ wrapped };
}

//==============================================================================
// Common angles
//==============================================================================

constexpr Angle Angle::Zero()     noexcept { return Angle{ 0.0f         }; }
constexpr Angle Angle::Right()    noexcept { return Angle{ HALF_PI      }; }
constexpr Angle Angle::Straight() noexcept { return Angle{ PI           }; }
constexpr Angle Angle::Full()     noexcept { return Angle{ TWO_PI       }; }

//==============================================================================
// Member operators
//==============================================================================

constexpr Angle& Angle::operator+=(Angle rhs) noexcept {
    m_radians += rhs.m_radians;
    return *this;
}

constexpr Angle& Angle::operator-=(Angle rhs) noexcept {
    m_radians -= rhs.m_radians;
    return *this;
}

constexpr Angle& Angle::operator*=(float scalar) noexcept {
    m_radians *= scalar;
    return *this;
}

constexpr Angle& Angle::operator/=(float scalar) noexcept {
    if (scalar != 0.0f && scalar == scalar)
        m_radians /= scalar;
    return *this;
}

constexpr Angle Angle::operator-() const noexcept {
    return Angle{ -m_radians };
}

constexpr bool Angle::operator==(Angle rhs) const noexcept { return m_radians == rhs.m_radians; }
constexpr bool Angle::operator!=(Angle rhs) const noexcept { return m_radians != rhs.m_radians; }
constexpr bool Angle::operator< (Angle rhs) const noexcept { return m_radians <  rhs.m_radians; }
constexpr bool Angle::operator> (Angle rhs) const noexcept { return m_radians >  rhs.m_radians; }
constexpr bool Angle::operator<=(Angle rhs) const noexcept { return m_radians <= rhs.m_radians; }
constexpr bool Angle::operator>=(Angle rhs) const noexcept { return m_radians >= rhs.m_radians; }

//==============================================================================
// Non-member arithmetic operators
//==============================================================================

constexpr Angle operator+(Angle lhs, Angle rhs) noexcept { lhs += rhs; return lhs; }
constexpr Angle operator-(Angle lhs, Angle rhs) noexcept { lhs -= rhs; return lhs; }
constexpr Angle operator*(Angle lhs, float scalar) noexcept { lhs *= scalar; return lhs; }
constexpr Angle operator*(float scalar, Angle rhs) noexcept { rhs *= scalar; return rhs; }
constexpr Angle operator/(Angle lhs, float scalar) noexcept { lhs /= scalar; return lhs; }

constexpr float operator/(Angle lhs, Angle rhs) noexcept {
    if (rhs.AsRadians() == 0.0f) return 0.0f;
    return lhs.AsRadians() / rhs.AsRadians();
}

//==============================================================================
// Stream output
//==============================================================================

inline std::ostream& operator<<(std::ostream& os, Angle a) {
    return os << a.AsRadians() << " rad (" << a.AsDegrees() << "\xC2\xB0)";
}

//==============================================================================
// Literal suffixes
//==============================================================================

namespace Literals {

constexpr Angle operator""_deg(long double degrees) noexcept {
    return Angle::Degrees(static_cast<float>(degrees));
}

constexpr Angle operator""_deg(unsigned long long degrees) noexcept {
    return Angle::Degrees(static_cast<float>(degrees));
}

constexpr Angle operator""_rad(long double radians) noexcept {
    return Angle::Radians(static_cast<float>(radians));
}

constexpr Angle operator""_rad(unsigned long long radians) noexcept {
    return Angle::Radians(static_cast<float>(radians));
}

} // namespace Literals

} // namespace RS::Math
