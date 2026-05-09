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
 *   Color.inl - Inline and constexpr implementations for RS::Color
 *
 *   This file is auto-included at the bottom of Color.hpp. Do not include it directly.
 **********************************************************************************************/

#pragma once

#include "Raysim/Graphics/Color.hpp"    // NOLINT(misc-header-include-cycle)
#include "Raysim/Core/Assert.hpp"
#include "Raysim/Math/Utils/MathUtils.hpp"

#include <stdexcept>
#include <string>

//==============================================================================
// File-local helpers
//==============================================================================

namespace {

/// Clamps an integer to [0, 255] and casts to uint8_t. Used by all saturating
/// arithmetic operators to prevent wrap-around without pulling in <algorithm>.
[[nodiscard]] constexpr std::uint8_t clampByte(int v) noexcept
{
    return static_cast<std::uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v));
}

/// Rounds a non-negative float to the nearest integer and clamps to [0, 255].
[[nodiscard]] inline std::uint8_t roundByte(float v) noexcept
{
    return clampByte(static_cast<int>(v + 0.5f));
}

} // anonymous namespace

//==============================================================================
// RS::Color -- Constructors
//==============================================================================

namespace RS {

constexpr Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                       std::uint8_t alpha) noexcept
    : r(red), g(green), b(blue), a(alpha)
{}

constexpr Color::Color(std::uint32_t rgba) noexcept
    : r(static_cast<std::uint8_t>((rgba >> 24u) & 0xFFu))
    , g(static_cast<std::uint8_t>((rgba >> 16u) & 0xFFu))
    , b(static_cast<std::uint8_t>((rgba >>  8u) & 0xFFu))
    , a(static_cast<std::uint8_t>( rgba         & 0xFFu))
{}

//==============================================================================
// RS::Color -- Factories
//==============================================================================

inline Color Color::Lerp(const Color& x, const Color& y, float t) noexcept
{
    const float ct = Math::Clamp01(t);
    return {
        roundByte(Math::LerpUnclamped(static_cast<float>(x.r), static_cast<float>(y.r), ct)),
        roundByte(Math::LerpUnclamped(static_cast<float>(x.g), static_cast<float>(y.g), ct)),
        roundByte(Math::LerpUnclamped(static_cast<float>(x.b), static_cast<float>(y.b), ct)),
        roundByte(Math::LerpUnclamped(static_cast<float>(x.a), static_cast<float>(y.a), ct))
    };
}

inline Color Color::FromHex(const std::string& hex)
{
    RS_ASSERT(!hex.empty() && hex[0] == '#', "Color::FromHex -- string must start with '#'");

    try {
        const std::string digits = hex.substr(1);
        RS_ASSERT(digits.size() == 6u || digits.size() == 8u,
                  "Color::FromHex -- expected 6 or 8 hex digits after '#'");

        const unsigned long value = std::stoul(digits, nullptr, 16);

        if (digits.size() == 8u) {
            return {
                static_cast<std::uint8_t>((value >> 24u) & 0xFFu),
                static_cast<std::uint8_t>((value >> 16u) & 0xFFu),
                static_cast<std::uint8_t>((value >>  8u) & 0xFFu),
                static_cast<std::uint8_t>( value         & 0xFFu)
            };
        }

        return {
            static_cast<std::uint8_t>((value >> 16u) & 0xFFu),
            static_cast<std::uint8_t>((value >>  8u) & 0xFFu),
            static_cast<std::uint8_t>( value         & 0xFFu),
            255u
        };
    }
    catch (const std::exception& e) {
        RS_CORE_ERROR("Color::FromHex -- failed to parse '{}': {}", hex, e.what());
        return {};
    }
}

inline Color Color::FromHSV(float h, float s, float v, std::uint8_t alpha) noexcept
{
    s = Math::Clamp01(s);
    v = Math::Clamp01(v);

    if (s <= 0.0f) {
        // Achromatic -- any hue maps to grey
        const std::uint8_t grey = roundByte(v * 255.0f);
        return { grey, grey, grey, alpha };
    }

    // Wrap hue to [0, 360)
    h = Math::Repeat(h, 360.0f);

    // Compute the sector index and fractional offset within that sector
    const float hNorm  = h / 60.0f;
    const int   sector = static_cast<int>(hNorm);
    const float frac   = hNorm - static_cast<float>(sector);

    const float p = v * (1.0f - s);
    const float q = v * (1.0f - s * frac);
    const float t = v * (1.0f - s * (1.0f - frac));

    float red{}, green{}, blue{};

    switch (sector % 6) {
        case 0:  red = v; green = t; blue = p; break;
        case 1:  red = q; green = v; blue = p; break;
        case 2:  red = p; green = v; blue = t; break;
        case 3:  red = p; green = q; blue = v; break;
        case 4:  red = t; green = p; blue = v; break;
        default: red = v; green = p; blue = q; break;
    }

    return {
        roundByte(red   * 255.0f),
        roundByte(green * 255.0f),
        roundByte(blue  * 255.0f),
        alpha
    };
}

inline Color Color::FromHSV(const HSV& hsv, std::uint8_t alpha) noexcept
{
    return FromHSV(hsv.h, hsv.s, hsv.v, alpha);
}

//==============================================================================
// RS::Color -- Conversion
//==============================================================================

inline std::string Color::ToHex(bool includeAlpha) const
{
    static constexpr char kHex[] = "0123456789ABCDEF";

    const std::size_t len = includeAlpha ? 9u : 7u;
    std::string result(len, '#');

    result[1] = kHex[(r >> 4u) & 0xFu];
    result[2] = kHex[ r        & 0xFu];
    result[3] = kHex[(g >> 4u) & 0xFu];
    result[4] = kHex[ g        & 0xFu];
    result[5] = kHex[(b >> 4u) & 0xFu];
    result[6] = kHex[ b        & 0xFu];

    if (includeAlpha) {
        result[7] = kHex[(a >> 4u) & 0xFu];
        result[8] = kHex[ a        & 0xFu];
    }

    return result;
}

inline Color::HSV Color::ToHSV() const noexcept
{
    const float rf = static_cast<float>(r) / 255.0f;
    const float gf = static_cast<float>(g) / 255.0f;
    const float bf = static_cast<float>(b) / 255.0f;

    const float cMax = Math::MaxValue(Math::MaxValue(rf, gf), bf);
    const float cMin = Math::MinValue(Math::MinValue(rf, gf), bf);
    const float diff = cMax - cMin;

    // Compute Value
    const float value = cMax;

    // Compute Saturation
    const float sat = (cMax > 0.0f) ? (diff / cMax) : 0.0f;

    // Compute Hue
    float hue = 0.0f;
    if (diff > 0.0f) {
        if (cMax == rf) {
            hue = 60.0f * Math::Repeat((gf - bf) / diff, 6.0f);
        } else if (cMax == gf) {
            hue = 60.0f * (((bf - rf) / diff) + 2.0f);
        } else {
            hue = 60.0f * (((rf - gf) / diff) + 4.0f);
        }
    }

    return { hue, sat, value };
}

constexpr std::uint32_t Color::ToRGBA32() const noexcept
{
    return (static_cast<std::uint32_t>(r) << 24u)
         | (static_cast<std::uint32_t>(g) << 16u)
         | (static_cast<std::uint32_t>(b) <<  8u)
         |  static_cast<std::uint32_t>(a);
}

constexpr Color Color::WithAlpha(std::uint8_t alpha) const noexcept
{
    return { r, g, b, alpha };
}

//==============================================================================
// RS::Color -- Arithmetic operators
//==============================================================================

constexpr Color Color::operator+(const Color& rhs) const noexcept
{
    return {
        clampByte(static_cast<int>(r) + static_cast<int>(rhs.r)),
        clampByte(static_cast<int>(g) + static_cast<int>(rhs.g)),
        clampByte(static_cast<int>(b) + static_cast<int>(rhs.b)),
        clampByte(static_cast<int>(a) + static_cast<int>(rhs.a))
    };
}

constexpr Color Color::operator-(const Color& rhs) const noexcept
{
    return {
        clampByte(static_cast<int>(r) - static_cast<int>(rhs.r)),
        clampByte(static_cast<int>(g) - static_cast<int>(rhs.g)),
        clampByte(static_cast<int>(b) - static_cast<int>(rhs.b)),
        clampByte(static_cast<int>(a) - static_cast<int>(rhs.a))
    };
}

constexpr Color Color::operator*(const Color& rhs) const noexcept
{
    // Modulate blend: (a * b) / 255, result always in [0, 255]
    return {
        static_cast<std::uint8_t>((static_cast<std::uint16_t>(r) * rhs.r) / 255u),
        static_cast<std::uint8_t>((static_cast<std::uint16_t>(g) * rhs.g) / 255u),
        static_cast<std::uint8_t>((static_cast<std::uint16_t>(b) * rhs.b) / 255u),
        static_cast<std::uint8_t>((static_cast<std::uint16_t>(a) * rhs.a) / 255u)
    };
}

inline Color Color::operator*(float scalar) const noexcept
{
    return {
        roundByte(static_cast<float>(r) * scalar),
        roundByte(static_cast<float>(g) * scalar),
        roundByte(static_cast<float>(b) * scalar),
        roundByte(static_cast<float>(a) * scalar)
    };
}

inline Color Color::operator/(float scalar) const noexcept
{
    if (scalar == 0.0f) return *this;
    return *this * (1.0f / scalar);
}

constexpr Color& Color::operator+=(const Color& rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

constexpr Color& Color::operator-=(const Color& rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

constexpr Color& Color::operator*=(const Color& rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

inline Color& Color::operator*=(float scalar) noexcept
{
    *this = *this * scalar;
    return *this;
}

inline Color& Color::operator/=(float scalar) noexcept
{
    *this = *this / scalar;
    return *this;
}

//==============================================================================
// RS::Color -- Comparison operators
//==============================================================================

constexpr bool Color::operator==(const Color& rhs) const noexcept
{
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

constexpr bool Color::operator!=(const Color& rhs) const noexcept
{
    return !(*this == rhs);
}

//==============================================================================
// Non-member operators
//==============================================================================

inline Color operator*(float scalar, const Color& c) noexcept
{
    return c * scalar;
}

inline std::ostream& operator<<(std::ostream& os, const Color& c)
{
    return os << "Color("
              << static_cast<int>(c.r) << ", "
              << static_cast<int>(c.g) << ", "
              << static_cast<int>(c.b) << ", "
              << static_cast<int>(c.a) << ')';
}

} // namespace RS

//==============================================================================
// Predefined colors
//==============================================================================

namespace RS::Colors
{
    inline constexpr Color White     {255u, 255u, 255u};
    inline constexpr Color Black     {0u,   0u,   0u  };
    inline constexpr Color Gray      {128u, 128u, 128u};
    inline constexpr Color LightGray {200u, 200u, 200u};

    inline constexpr Color Red       {255u, 0u,   0u  };
    inline constexpr Color Green     {0u,   255u, 0u  };
    inline constexpr Color Blue      {0u,   0u,   255u};

    inline constexpr Color Yellow    {255u, 255u, 0u  };
    inline constexpr Color Cyan      {0u,   255u, 255u};
    inline constexpr Color Magenta   {255u, 0u,   255u};

    inline constexpr Color Orange    {255u, 165u, 0u  };
    inline constexpr Color Pink      {255u, 192u, 203u};
    inline constexpr Color Purple    {128u, 0u,   128u};

    // Raylib-style colors
    inline constexpr Color RayRed    {239u, 68u,  68u };
    inline constexpr Color RayGreen  {89u,  255u, 128u};
    inline constexpr Color RayBlue   {0u,   161u, 224u};
    inline constexpr Color RayYellow {255u, 229u, 0u  };
    inline constexpr Color RayBlack  {23u,  26u,  31u };
    inline constexpr Color RayWhite  {221u, 221u, 221u};
    inline constexpr Color RayGray   {64u,  64u,  64u };
    inline constexpr Color RayCyan   {89u,  255u, 241u};
    inline constexpr Color DarkBlue  {40u,  44u,  52u };

    inline constexpr Color Transparent {0u, 0u, 0u, 0u};
} // namespace RS::Colors
