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

#include "Raysim/Math/Types/Vector2.hpp"

namespace RS {

/**
 * @enum OriginMode
 * @brief Defines the origin point (anchor) for drawing operations
 *
 * Specifies how coordinates are interpreted when drawing shapes and text.
 * Each draw function uses TopLeft (classic raylib behavior) by default when
 * calling the basic overload, but you can specify an alternative origin.
 *
 * The origin modes form a 3x3 grid:
 * ```
 * TopLeft    - TopCenter    - TopRight
 *    |             |             |
 * CenterLeft -  Center     - CenterRight
 *    |             |             |
 * BottomLeft - BottomCenter - BottomRight
 * ```
 *
 * @note Default behavior (when not specifying origin) depends on the backend/renderer.
 * For example, raylib's default is TopLeft, while some other backends may use Center.
 */
enum class OriginMode {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// ============================================================================
// Helper Functions
// ============================================================================

/// @brief Convert OriginMode to a normalized origin vector in [0..1].
///
/// Maps each OriginMode to a 2D offset vector where:
/// - x in [0, 1]: 0=left, 0.5=center, 1=right
/// - y in [0, 1]: 0=top, 0.5=center, 1=bottom
///
/// @param origin The OriginMode to convert.
/// @return A Vec2 representing the normalized offset.
///
/// Example:
/// @code
/// auto v = OriginToVector(OriginMode::Center);  // Returns {0.5f, 0.5f}
/// auto v = OriginToVector(OriginMode::TopLeft); // Returns {0.0f, 0.0f}
/// @endcode
[[nodiscard]] constexpr Math::Vec2f OriginToVector(OriginMode origin) noexcept
{
    float x = 0.0f;
    float y = 0.0f;

    switch (origin) {
        case OriginMode::TopLeft:      x = 0.0f; y = 0.0f; break;
        case OriginMode::TopCenter:    x = 0.5f; y = 0.0f; break;
        case OriginMode::TopRight:     x = 1.0f; y = 0.0f; break;
        case OriginMode::CenterLeft:   x = 0.0f; y = 0.5f; break;
        case OriginMode::Center:       x = 0.5f; y = 0.5f; break;
        case OriginMode::CenterRight:  x = 1.0f; y = 0.5f; break;
        case OriginMode::BottomLeft:   x = 0.0f; y = 1.0f; break;
        case OriginMode::BottomCenter: x = 0.5f; y = 1.0f; break;
        case OriginMode::BottomRight:  x = 1.0f; y = 1.0f; break;
        default:                       x = 0.0f; y = 0.0f; break;
    }

    return { x, y };
}

} // namespace RS
