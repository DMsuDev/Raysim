#pragma once

#include <string>
#include "Raysim/Math/Vector3.hpp"

namespace RS {

/**
 * @struct Color
 * @brief RGBA color representation with predefined constants
 *
 * Represents a color with red, green, blue, and alpha components (0-255).
 * Provides common operations like color interpolation and component arithmetic.
 */
struct Color {
    unsigned char r{255}, g{255}, b{255}, a{255};  ///< Red, Green, Blue, Alpha components

//=======================================================
// Constructors
//=======================================================

    constexpr Color() = default;

    /**
     * @brief Construct a Color from RGBA components
     * @param in_r Red component (0-255)
     * @param in_g Green component (0-255)
     * @param in_b Blue component (0-255)
     * @param in_a Alpha component (0-255, default 255)
     */
    constexpr Color(unsigned char in_r, unsigned char in_g, unsigned char in_b, unsigned char in_a = 255)
        : r(in_r), g(in_g), b(in_b), a(in_a) {}

    /**
     * @brief Construct grayscale color (equal R, G, B)
     * @param gray Grayscale value
     * @param in_a Alpha component (default 255)
     */
    explicit constexpr Color(unsigned char gray, unsigned char in_a = 255)
        : r(gray), g(gray), b(gray), a(in_a) {}

//=======================================================
// Operators
//=======================================================

    Color operator+(const Color& rhs) const;
    Color operator-(const Color& rhs) const;

    bool operator==(const Color& rhs) const
    {
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }
    bool operator!=(const Color& rhs) const { return !(*this == rhs); }

//=======================================================
// Interpolation
//=======================================================

    /**
     * @brief Linear interpolate between two colors
     * @param a Start color
     * @param b End color
     * @param t Interpolation factor (clamped to 0-1)
     * @return Interpolated color
     */
    [[nodiscard]] static Color Lerp(const Color& a, const Color& b, float t);

//=======================================================
// Conversion utilities
//=======================================================

    /**
     * @brief Create a Color from a hexadecimal string (e.g. "#RRGGBB" or "#RRGGBBAA")
     * @param hex Hexadecimal color string
     * @return Parsed Color object
     */
    [[nodiscard]] static Color FromHex(const std::string& hex);

    /**
     * @brief Create a Color from HSV values
     * @param h Hue (0-360 degrees)
     * @param s Saturation (0-1)
     * @param v Value (0-1)
     * @param alpha Optional alpha component (0-255, default 255)
     * @return Color object representing the HSV values
     */
    [[nodiscard]] static Color FromHSV(float h, float s, float v, unsigned char alpha = 255);

    /**
     * @brief Create a Color from HSV values using a Vector3 (hue, saturation, value)
     * @param hsv Vector3 where x = hue (0-360), y = saturation
     * z = value (0-1)
     * @param alpha Optional alpha component (0-255, default 255)
     * @return Color object representing the HSV values
     */
    [[nodiscard]] static Color FromHSV(const Vector3& hsv, unsigned char alpha = 255)
    {
        return FromHSV(hsv.x, hsv.y, hsv.z, alpha);
    }

    /**
     * @brief Convert this Color to a hexadecimal string (e.g. "#RRGGBBAA")
     * @param includeAlpha Whether to include the alpha component in the hex string (default false)
     * @return Hexadecimal string representation of the color
     */
    [[nodiscard]] std::string ToHex(bool includeAlpha = false) const;

    /**
    * @brief Convert this Color to HSV representation
    * @return Vector3 where x = hue (0-360), y = saturation (0-1), z = value (0-1)
    */
    [[nodiscard]] Vector3 ToHSV() const;
};

//=======================================================
// PREDEFINED COLORS (ENGINE STYLE)
//=======================================================

namespace Colors
{
    inline constexpr Color White      {255, 255, 255};
    inline constexpr Color Black      {0, 0, 0};
    inline constexpr Color Gray       {128, 128, 128};
    inline constexpr Color LightGray  {200, 200, 200};

    inline constexpr Color Red        {255, 0, 0};
    inline constexpr Color Green      {0, 255, 0};
    inline constexpr Color Blue       {0, 0, 255};

    inline constexpr Color Yellow     {255, 255, 0};
    inline constexpr Color Cyan       {0, 255, 255};
    inline constexpr Color Magenta    {255, 0, 255};

    inline constexpr Color Orange     {255, 165, 0};
    inline constexpr Color Pink       {255, 192, 203};
    inline constexpr Color Purple     {128, 0, 128};

    // Raylib-style colors
    inline constexpr Color RayRed     {239, 68, 68};
    inline constexpr Color RayGreen   {89, 255, 128};
    inline constexpr Color RayBlue    {0, 161, 224};
    inline constexpr Color RayYellow  {255, 229, 0};

    inline constexpr Color RayBlack   {23, 26, 31};
    inline constexpr Color RayWhite   {221, 221, 221};
    inline constexpr Color RayGray    {64, 64, 64};
    inline constexpr Color RayCyan    {89, 255, 241};

    inline constexpr Color DarkBlue   {40, 44, 52};
}

} // namespace RS
