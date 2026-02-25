#pragma once

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

    constexpr Color() = default;

    /// @brief Construct color from RGBA components
    constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}

    /// @brief Construct grayscale color (equal R, G, B)
    explicit constexpr Color(unsigned char gray, unsigned char a = 255)
        : r(gray), g(gray), b(gray), a(a) {}

    /**
     * @brief Linear interpolate between two colors
     * @param a Start color
     * @param b End color
     * @param t Interpolation factor (clamped to 0-1)
     * @return Interpolated color
     */
    static Color Lerp(const Color& a, const Color& b, float t);

    // ==== OPERATORS =====

    /// @brief Add color components (bright blend)
    Color operator+(const Color& rhs) const;
    /// @brief Subtract color components (dark blend)
    Color operator-(const Color& rhs) const;

    /// @brief Equality comparison
    bool operator==(const Color& rhs) const
    {
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }
    /// @brief Inequality comparison
    bool operator!=(const Color& rhs) const { return !(*this == rhs); }

    // ==== PREDEFINED COLORS =====

    static constexpr Color Red()          { return {255,   0,   0}; }
    static constexpr Color Green()        { return {  0, 255,   0}; }
    static constexpr Color Yellow()       { return {255, 255,   0}; }
    static constexpr Color Blue()         { return {  0,   0, 255}; }

    static constexpr Color White()        { return {255, 255, 255}; }
    static constexpr Color Black()        { return {  0,   0,   0}; }
    static constexpr Color Gray()         { return {128, 128, 128}; }

    static constexpr Color Cyan()         { return {  0, 255, 255}; }
    static constexpr Color Magenta()      { return {255,   0, 255}; }
    static constexpr Color Pink()         { return {255, 192, 203}; }
    static constexpr Color Orange()       { return {255, 165,   0}; }
    static constexpr Color Purple()       { return {128,   0, 128}; }

    // ==== RAYSIM (AND RAYLIB WRAPPED) COLORS =====
    static constexpr Color RayRed()       { return {239,  68,  68}; }
    static constexpr Color RayGreen()     { return { 89, 255, 128}; }
    static constexpr Color RayBlue()      { return {  0, 161, 224}; }
    static constexpr Color RayYellow()    { return {255, 229,   0}; }

    static constexpr Color RayBlack()     { return { 23,  26,  31}; }
    static constexpr Color RayWhite()     { return {221, 221, 221}; }

    static constexpr Color RayCyan()      { return { 89, 255, 241}; }
    static constexpr Color RayGray()     { return { 64,  64,  64}; }
    static constexpr Color LightGray()    { return {200, 200, 200}; }
    static constexpr Color DarkBlue()     { return { 40,  44,  52}; }
};

} // namespace RS
