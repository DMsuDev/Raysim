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
 *   RS::Color - 32-bit RGBA color type
 *
 *   Each channel (red, green, blue, alpha) is stored as a `uint8_t` in [0, 255].
 *   All arithmetic operators clamp results to [0, 255] so values can never overflow
 *   or underflow silently.
 *
 *   Supported operations:
 *     - Construction from RGBA bytes, packed uint32 (0xRRGGBBAA), and hex strings
 *     - Conversion to/from HSV color space
 *     - Linear interpolation (Lerp)
 *     - Saturating addition / subtraction (+ -)
 *     - Modulate blending for Color * Color (component-wise multiply / 255)
 *     - Scalar multiplication and division (* /)
 *     - Equality comparison (== !=)
 **********************************************************************************************/

#pragma once

#include <cstdint>
#include <string>

namespace RS {

/**
 * @struct Color
 * @brief 32-bit RGBA color with saturating arithmetic and conversion utilities.
 *
 * Each component is stored as an unsigned byte in [0, 255]. All arithmetic
 * operators clamp their results to this range, preventing silent wrap-around.
 *
 * Example:
 * @code
 *     Color red  = Colors::Red;
 *     Color blue = Colors::Blue;
 *
 *     Color lerped = Color::Lerp(red, blue, 0.5f);  // purple
 *     Color faded  = red * 0.5f;                    // dark red
 *     Color hex    = Color::FromHex("#FF8800");   // orange
 * @endcode
 */
struct Color {

    std::uint8_t r{0};   ///< Red   channel (0 = none, 255 = full).
    std::uint8_t g{0};   ///< Green channel (0 = none, 255 = full).
    std::uint8_t b{0};   ///< Blue  channel (0 = none, 255 = full).
    std::uint8_t a{255}; ///< Alpha channel (0 = transparent, 255 = opaque).

    /**
     * @struct HSV
     * @brief HSV (Hue, Saturation, Value) color representation.
     *
     * Used by `ToHSV()` and `FromHSV()` to convert between the RGB and HSV
     * color spaces. All fields follow the standard conventions:
     *   - `h` -- hue in [0, 360) degrees.
     *   - `s` -- saturation in [0, 1].
     *   - `v` -- value (brightness) in [0, 1].
     */
    struct HSV {
        float h{0.0f}; ///< Hue in [0, 360) degrees.
        float s{0.0f}; ///< Saturation in [0, 1].
        float v{0.0f}; ///< Value (brightness) in [0, 1].
    };

//==============================================================================
// Constructors
//==============================================================================

    /**
     * @brief Default constructor -- opaque black.
     *
     * Initializes all components to 0 except alpha which defaults to 255.
     * Equivalent to `Color(0, 0, 0, 255)`.
     */
    constexpr Color() noexcept = default;

    /**
     * @brief Constructs a Color from individual RGBA byte components.
     * @param red   Red   component (0-255).
     * @param green Green component (0-255).
     * @param blue  Blue  component (0-255).
     * @param alpha Alpha component (0-255, default 255 = fully opaque).
     */
    constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                    std::uint8_t alpha = 255) noexcept;

    /**
     * @brief Constructs a Color from a packed 32-bit RGBA integer.
     *
     * The integer layout is `0xRRGGBBAA`, where the red channel occupies the
     * most-significant byte and alpha the least-significant.
     *
     * @param rgba Packed 32-bit color value.
     */
    constexpr explicit Color(std::uint32_t rgba) noexcept;

//==============================================================================
// Factories
//==============================================================================

    /**
     * @brief Linearly interpolates between two colors by factor `t`.
     *
     * `t` is clamped to [0, 1]. Each channel is interpolated independently
     * and rounded to the nearest integer.
     *
     * @param a Start color (returned when `t == 0`).
     * @param b End   color (returned when `t == 1`).
     * @param t Interpolation factor, clamped to [0, 1].
     * @return Interpolated color with all channels in [0, 255].
     */
    [[nodiscard]] static Color Lerp(const Color& a, const Color& b, float t) noexcept;

    /**
     * @brief Parses a Color from a hexadecimal CSS-style string.
     *
     * Accepted formats: `"#RRGGBB"` (alpha defaults to 255) or `"#RRGGBBAA"`.
     * The leading `#` is required. Letters may be upper or lower case.
     *
     * @param hex Hexadecimal string in `"#RRGGBB"` or `"#RRGGBBAA"` format.
     * @return Parsed Color. Returns opaque black on parse failure.
     */
    [[nodiscard]] static Color FromHex(const std::string& hex);

    /**
     * @brief Constructs a Color from HSV components.
     *
     * Converts the given Hue/Saturation/Value triplet to an RGB color.
     * `h` is wrapped to [0, 360), `s` and `v` are clamped to [0, 1].
     *
     * @param h     Hue in degrees (any value, wrapped to [0, 360)).
     * @param s     Saturation in [0, 1].
     * @param v     Value (brightness) in [0, 1].
     * @param alpha Alpha channel (0-255, default 255).
     * @return Equivalent RGBA Color.
     */
    [[nodiscard]] static Color FromHSV(float h, float s, float v, std::uint8_t alpha = 255) noexcept;

    /**
     * @brief Constructs a Color from an HSV struct.
     *
     * Convenience overload that forwards to `FromHSV(float, float, float, uint8_t)`.
     *
     * @param hsv   HSV struct (h in [0, 360), s and v in [0, 1]).
     * @param alpha Alpha channel (0-255, default 255).
     * @return Equivalent RGBA Color.
     */
    [[nodiscard]] static Color FromHSV(const HSV& hsv, std::uint8_t alpha = 255) noexcept;

//==============================================================================
// Conversion
//==============================================================================

    /**
     * @brief Converts this Color to a CSS-style hexadecimal string.
     *
     * Returns `"#RRGGBB"` by default. Pass `includeAlpha = true` to get
     * `"#RRGGBBAA"`. Letters are upper case.
     *
     * @param includeAlpha Whether to append the alpha byte (default false).
     * @return Hexadecimal string representation.
     */
    [[nodiscard]] std::string ToHex(bool includeAlpha = false) const;

    /**
     * @brief Converts this Color to its HSV representation.
     *
     * @return HSV struct where `h` is in [0, 360), `s` and `v` are in [0, 1].
     */
    [[nodiscard]] HSV ToHSV() const noexcept;

    /**
     * @brief Packs this Color into a 32-bit integer in `0xRRGGBBAA` format.
     *
     * The red channel occupies the most-significant byte; alpha occupies the
     * least-significant byte.
     *
     * @return Packed 32-bit RGBA value.
     */
    [[nodiscard]] constexpr std::uint32_t ToRGBA32() const noexcept;

    /**
     * @brief Returns a copy of this color with a different alpha value.
     *
     * Useful for creating transparent variants of existing colors without
     * modifying the original.
     *
     * @param alpha New alpha component (0-255).
     * @return Copy of this color with `a` replaced by `alpha`.
     */
    [[nodiscard]] constexpr Color WithAlpha(std::uint8_t alpha) const noexcept;

//==============================================================================
// Arithmetic operators
//==============================================================================

    /**
     * @brief Saturating per-channel addition.
     *
     * Adds each channel independently, clamping the result to [0, 255].
     * No channel can overflow.
     *
     * @param rhs Color to add.
     * @return New Color with clamped channel sums.
     */
    [[nodiscard]] constexpr Color operator+(const Color& rhs) const noexcept;

    /**
     * @brief Saturating per-channel subtraction.
     *
     * Subtracts each channel independently, clamping the result to [0, 255].
     * No channel can underflow below 0.
     *
     * @param rhs Color to subtract.
     * @return New Color with clamped channel differences.
     */
    [[nodiscard]] constexpr Color operator-(const Color& rhs) const noexcept;

    /**
     * @brief Per-channel modulate blend (Color * Color).
     *
     * Multiplies each corresponding pair of channels and divides by 255,
     * producing the standard "modulate" blend used in texture tinting:
     *
     *     result.r = (this->r * rhs.r) / 255
     *
     * The result is always within [0, 255].
     *
     * @param rhs Color to modulate with.
     * @return Modulated color.
     */
    [[nodiscard]] constexpr Color operator*(const Color& rhs) const noexcept;

    /**
     * @brief Scales all channels by a scalar factor.
     *
     * Each channel is multiplied by `scalar`, rounded to the nearest integer,
     * and clamped to [0, 255]. Useful for adjusting brightness or opacity.
     *
     * @param scalar Scale factor (e.g. 0.5 for half brightness).
     * @return New Color with all channels scaled and clamped.
     */
    [[nodiscard]] Color operator*(float scalar) const noexcept;

    /**
     * @brief Divides all channels by a scalar factor.
     *
     * Equivalent to `*this * (1.0f / scalar)`. If `scalar` is zero,
     * the color is returned unchanged.
     *
     * @param scalar Divisor (non-zero).
     * @return New Color with all channels divided and clamped.
     */
    [[nodiscard]] Color operator/(float scalar) const noexcept;

    /**
     * @brief Saturating in-place addition.
     * @param rhs Color to add.
     * @return Reference to this color after addition.
     */
    constexpr Color& operator+=(const Color& rhs) noexcept;

    /**
     * @brief Saturating in-place subtraction.
     * @param rhs Color to subtract.
     * @return Reference to this color after subtraction.
     */
    constexpr Color& operator-=(const Color& rhs) noexcept;

    /**
     * @brief In-place modulate blend.
     * @param rhs Color to modulate with.
     * @return Reference to this color after modulation.
     */
    constexpr Color& operator*=(const Color& rhs) noexcept;

    /**
     * @brief In-place scalar scale.
     * @param scalar Scale factor.
     * @return Reference to this color after scaling.
     */
    Color& operator*=(float scalar) noexcept;

    /**
     * @brief In-place scalar division.
     *
     * If `scalar` is zero, the color is left unchanged.
     *
     * @param scalar Divisor (non-zero).
     * @return Reference to this color after division.
     */
    Color& operator/=(float scalar) noexcept;

//==============================================================================
// Comparison operators
//==============================================================================

    /**
     * @brief Checks whether all four channels are equal.
     * @param rhs Color to compare against.
     * @return True if r, g, b and a all match.
     */
    [[nodiscard]] constexpr bool operator==(const Color& rhs) const noexcept;

    /**
     * @brief Checks whether any channel differs.
     * @param rhs Color to compare against.
     * @return True if any channel does not match.
     */
    [[nodiscard]] constexpr bool operator!=(const Color& rhs) const noexcept;
};

//==============================================================================
// Non-member operators
//==============================================================================

/**
 * @brief Scales a color by a scalar (scalar on the left).
 *
 * Equivalent to `c * scalar`. Allows natural expressions like `0.5f * color`.
 *
 * @param scalar Scale factor.
 * @param c      Color to scale.
 * @return Scaled color with all channels clamped to [0, 255].
 */
[[nodiscard]] inline Color operator*(float scalar, const Color& c) noexcept;

/**
 * @brief Writes a Color to an output stream as `Color(r, g, b, a)`.
 * @param os Output stream.
 * @param c  Color to write.
 * @return Reference to `os`.
 */
inline std::ostream& operator<<(std::ostream& os, const Color& c);

} // namespace RS

#include "Raysim/Graphics/Color.inl"
