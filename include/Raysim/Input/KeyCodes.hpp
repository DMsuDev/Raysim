#pragma once

#include <cstdint>

namespace RS {

/**
 * @enum KeyCode
 * @brief Keyboard key codes (mirrors raylib KeyboardKey)
 */
enum class KeyCode : uint16_t {
    Null = 0,

    // Alphanumeric keys

    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Zero = 48, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

    // Function keys

    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Special keys

    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Semicolon = 59,
    Equal = 61,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    Grave = 96,

    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,

    // Modifier keys

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,

    // Special state keys

    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,

    // Numpad keys

    KP_0 = 320, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
    KP_Decimal = 330,
    KP_Divide = 331,
    KP_Multiply = 332,
    KP_Subtract = 333,
    KP_Add = 334,
    KP_Enter = 335,
    KP_Equal = 336,
};

/**
 * @enum MouseButton
 * @brief Mouse button codes
 *
 * Mouse buttons are typically indexed from 0 (left) to 2 (middle), but this enum provides named constants for clarity.
 */
enum class MouseButton : uint8_t {
    Left = 0,
    Right = 1,
    Middle = 2,
};

/**
 * @enum GamepadButton
 * @brief Gamepad button codes (Xbox-style)
 *
 * Defines common gamepad buttons including face buttons, triggers, and thumbstick presses.
 */
enum class GamepadButton : uint8_t {
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

    LB = 9,     // Left Bumper / Left Trigger 1
    LT = 10,    // Left Trigger 2
    RB = 11,    // Right Bumper / Right Trigger 1
    RT = 12,    // Right Trigger 2

    // Center buttons

    Back = 13,
    Guide = 14,
    Start = 15,

    // Thumb stick presses

    LeftStick = 16,
    RightStick = 17,
};

/**
 * @enum GamepadAxis
 * @brief Gamepad analog axis codes
 */
enum class GamepadAxis : uint8_t {
    LeftX = 0,
    LeftY = 1,
    RightX = 2,
    RightY = 3,
    LeftTrigger = 4,
    RightTrigger = 5,
};

} // namespace RS
