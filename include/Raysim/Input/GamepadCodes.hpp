#pragma once

#include <cstdint>

namespace RS
{
    /// @enum GamepadButton
    /// @brief Gamepad button codes (Xbox-style)
    /// Defines common gamepad buttons including face buttons, triggers, and thumbstick presses.
    enum class GamepadButton : uint8_t
    {
        Unknown = 0,

        // Left face buttons (D-Pad equivalent)

        Up = 1,
        Right = 2,
        Down = 3,
        Left = 4,

        // Right face buttons (Y, B, A, X)

        Y = 5,
        B = 6,
        A = 7,
        X = 8,

        // Triggers

        LB = 9,  // Left Bumper / Left Trigger 1
        LT = 10, // Left Trigger 2
        RB = 11, // Right Bumper / Right Trigger 1
        RT = 12, // Right Trigger 2

        // Center buttons

        Back = 13,
        Guide = 14,
        Start = 15,

        // Thumb stick presses

        LeftStick = 16,
        RightStick = 17,
    };

    /// @enum GamepadAxis
    /// @brief Gamepad analog axis codes
    enum class GamepadAxis : uint8_t
    {
        LeftX = 0,
        LeftY = 1,
        RightX = 2,
        RightY = 3,
        LeftTrigger = 4,
        RightTrigger = 5,
    };

} // namespace RS
