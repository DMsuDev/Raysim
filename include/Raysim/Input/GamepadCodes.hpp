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

#include <cstdint>

namespace RS {

// ============================================================================
// GamepadButton
// ============================================================================

/**
 * @enum GamepadButton
 * @brief Backend-agnostic gamepad button codes (Xbox-style layout).
 *
 * Defines a common set of button identifiers used internally by the input system.
 * Values are explicit and stable so backends can map their native constants to
 * this enum through a dedicated translation layer.
 *
 * Left/right face buttons follow the Xbox naming convention (A/B/X/Y).
 * D-pad directions are listed under the left face group.
 */
enum class GamepadButton : uint8_t
{
    Unknown = 0, ///< Unknown / unmapped button

    // -- D-Pad (left face) -----------------------------------------------------
    Up    = 1,
    Right = 2,
    Down  = 3,
    Left  = 4,

    // -- Face buttons (right face) ---------------------------------------------
    Y = 5, ///< Top    (Xbox Y  / PlayStation Triangle)
    B = 6, ///< Right  (Xbox B  / PlayStation Circle)
    A = 7, ///< Bottom (Xbox A  / PlayStation Cross)
    X = 8, ///< Left   (Xbox X  / PlayStation Square)

    // -- Shoulder / trigger buttons --------------------------------------------
    LB = 9,  ///< Left  Bumper   (L1 on PlayStation)
    LT = 10, ///< Left  Trigger  (L2 on PlayStation)
    RB = 11, ///< Right Bumper   (R1 on PlayStation)
    RT = 12, ///< Right Trigger  (R2 on PlayStation)

    // -- Center buttons --------------------------------------------------------
    Back  = 13, ///< Select / View
    Guide = 14, ///< Home / PS button
    Start = 15, ///< Start / Options

    // -- Thumbstick presses ----------------------------------------------------
    LeftStick  = 16, ///< Left  thumbstick click (L3)
    RightStick = 17, ///< Right thumbstick click (R3)

    Count ///< Sentinel: number of valid button codes (do not use as a button)
};

/// Total number of GamepadButton entries (0 = Unknown ... Count - 1 = RightStick).
inline constexpr std::size_t GamepadButtonCount = static_cast<std::size_t>(GamepadButton::Count);

// ============================================================================
// GamepadAxis
// ============================================================================

/**
 * @enum GamepadAxis
 * @brief Backend-agnostic gamepad analog axis codes.
 *
 * Covers the two thumbsticks and the analog triggers. All values are
 * normalized to [-1.0, 1.0] by the backend.
 *
 * Trigger axes typically rest at -1.0 (not pressed) and reach 1.0 (fully pressed).
 * Thumbstick axes rest near 0.0; positive X is right, positive Y is down
 * (matches screen-space convention used by most backends).
 */
enum class GamepadAxis : uint8_t
{
    // -- Left thumbstick -------------------------------------------------------
    LeftX = 0, ///< Horizontal: -1.0 = left,  +1.0 = right
    LeftY = 1, ///< Vertical:   -1.0 = up,    +1.0 = down

    // -- Right thumbstick ------------------------------------------------------
    RightX = 2, ///< Horizontal: -1.0 = left,  +1.0 = right
    RightY = 3, ///< Vertical:   -1.0 = up,    +1.0 = down

    // -- Analog triggers -------------------------------------------------------
    LeftTrigger  = 4, ///< -1.0 = released, +1.0 = fully pressed
    RightTrigger = 5, ///< -1.0 = released, +1.0 = fully pressed

    Count ///< Sentinel: number of valid axis codes (do not use as an axis)
};

/// Total number of GamepadAxis entries.
inline constexpr std::size_t GamepadAxisCount = static_cast<std::size_t>(GamepadAxis::Count);

} // namespace RS
