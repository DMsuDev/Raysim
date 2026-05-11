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

#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Input/MouseCodes.hpp"
#include "Raysim/Input/GamepadCodes.hpp"

#include <array>

namespace RS::Backend
{
    // ===========================================================================
    // KeyCode -> Raylib key integer mapping
    // ===========================================================================
    //
    // Raylib wraps GLFW physical key codes. GLFW key codes are layout-independent:
    // they identify the physical position of a key, not the character printed on
    // the keycap (e.g. KEY_A is always the key in the A-position, regardless of
    // whether the layout is QWERTY, AZERTY, Dvorak, etc.).
    //
    // This makes Raylib key integers a valid scancode proxy for this backend:
    //   GetScanCode(KeyCode::A) returns 65  (KEY_A / GLFW_KEY_A)
    //
    // Integer values listed here match raylib.h KEY_* constants exactly.
    // ===========================================================================

    inline constexpr std::array<int, static_cast<std::size_t>(KeyCode::Count)> KeyCodeToRaylib = {{
    //   Value   Index   KeyCode            Raylib constant
         0,   //   0    Unknown            KEY_NULL
        32,   //   1    Space              KEY_SPACE
        39,   //   2    Apostrophe         KEY_APOSTROPHE
        44,   //   3    Comma              KEY_COMMA
        45,   //   4    Minus              KEY_MINUS
        46,   //   5    Period             KEY_PERIOD
        47,   //   6    Slash              KEY_SLASH
        48,   //   7    D0                 KEY_ZERO
        49,   //   8    D1                 KEY_ONE
        50,   //   9    D2                 KEY_TWO
        51,   //  10    D3                 KEY_THREE
        52,   //  11    D4                 KEY_FOUR
        53,   //  12    D5                 KEY_FIVE
        54,   //  13    D6                 KEY_SIX
        55,   //  14    D7                 KEY_SEVEN
        56,   //  15    D8                 KEY_EIGHT
        57,   //  16    D9                 KEY_NINE
        59,   //  17    Semicolon          KEY_SEMICOLON
        61,   //  18    Equal              KEY_EQUAL
        65,   //  19    A                  KEY_A
        66,   //  20    B                  KEY_B
        67,   //  21    C                  KEY_C
        68,   //  22    D                  KEY_D
        69,   //  23    E                  KEY_E
        70,   //  24    F                  KEY_F
        71,   //  25    G                  KEY_G
        72,   //  26    H                  KEY_H
        73,   //  27    I                  KEY_I
        74,   //  28    J                  KEY_J
        75,   //  29    K                  KEY_K
        76,   //  30    L                  KEY_L
        77,   //  31    M                  KEY_M
        78,   //  32    N                  KEY_N
        79,   //  33    O                  KEY_O
        80,   //  34    P                  KEY_P
        81,   //  35    Q                  KEY_Q
        82,   //  36    R                  KEY_R
        83,   //  37    S                  KEY_S
        84,   //  38    T                  KEY_T
        85,   //  39    U                  KEY_U
        86,   //  40    V                  KEY_V
        87,   //  41    W                  KEY_W
        88,   //  42    X                  KEY_X
        89,   //  43    Y                  KEY_Y
        90,   //  44    Z                  KEY_Z
        91,   //  45    LeftBracket        KEY_LEFT_BRACKET
        92,   //  46    Backslash          KEY_BACKSLASH
        93,   //  47    RightBracket       KEY_RIGHT_BRACKET
        96,   //  48    GraveAccent        KEY_GRAVE
       256,   //  49    Escape             KEY_ESCAPE
       257,   //  50    Enter              KEY_ENTER
       258,   //  51    Tab                KEY_TAB
       259,   //  52    Backspace          KEY_BACKSPACE
       260,   //  53    Insert             KEY_INSERT
       261,   //  54    Delete             KEY_DELETE
       262,   //  55    Right              KEY_RIGHT
       263,   //  56    Left               KEY_LEFT
       264,   //  57    Down               KEY_DOWN
       265,   //  58    Up                 KEY_UP
       266,   //  59    PageUp             KEY_PAGE_UP
       267,   //  60    PageDown           KEY_PAGE_DOWN
       268,   //  61    Home               KEY_HOME
       269,   //  62    End                KEY_END
       280,   //  63    CapsLock           KEY_CAPS_LOCK
       281,   //  64    ScrollLock         KEY_SCROLL_LOCK
       282,   //  65    NumLock            KEY_NUM_LOCK
       283,   //  66    PrintScreen        KEY_PRINT_SCREEN
       284,   //  67    Pause              KEY_PAUSE
       290,   //  68    F1                 KEY_F1
       291,   //  69    F2                 KEY_F2
       292,   //  70    F3                 KEY_F3
       293,   //  71    F4                 KEY_F4
       294,   //  72    F5                 KEY_F5
       295,   //  73    F6                 KEY_F6
       296,   //  74    F7                 KEY_F7
       297,   //  75    F8                 KEY_F8
       298,   //  76    F9                 KEY_F9
       299,   //  77    F10                KEY_F10
       300,   //  78    F11                KEY_F11
       301,   //  79    F12                KEY_F12
       320,   //  80    KP0                KEY_KP_0
       321,   //  81    KP1                KEY_KP_1
       322,   //  82    KP2                KEY_KP_2
       323,   //  83    KP3                KEY_KP_3
       324,   //  84    KP4                KEY_KP_4
       325,   //  85    KP5                KEY_KP_5
       326,   //  86    KP6                KEY_KP_6
       327,   //  87    KP7                KEY_KP_7
       328,   //  88    KP8                KEY_KP_8
       329,   //  89    KP9                KEY_KP_9
       330,   //  90    KPDecimal          KEY_KP_DECIMAL
       331,   //  91    KPDivide           KEY_KP_DIVIDE
       332,   //  92    KPMultiply         KEY_KP_MULTIPLY
       333,   //  93    KPSubtract         KEY_KP_SUBTRACT
       334,   //  94    KPAdd              KEY_KP_ADD
       335,   //  95    KPEnter            KEY_KP_ENTER
       336,   //  96    KPEqual            KEY_KP_EQUAL
       340,   //  97    LeftShift          KEY_LEFT_SHIFT
       341,   //  98    LeftControl        KEY_LEFT_CONTROL
       342,   //  99    LeftAlt            KEY_LEFT_ALT
       343,   // 100    LeftSuper          KEY_LEFT_SUPER
       344,   // 101    RightShift         KEY_RIGHT_SHIFT
       345,   // 102    RightControl       KEY_RIGHT_CONTROL
       346,   // 103    RightAlt           KEY_RIGHT_ALT
       347,   // 104    RightSuper         KEY_RIGHT_SUPER
       348,   // 105    Menu               KEY_KB_MENU
    }};

    // ---------------------------------------------------------------------------
    // Compile-time validation
    // ---------------------------------------------------------------------------

    static_assert(
        KeyCodeToRaylib.size() == static_cast<std::size_t>(KeyCode::Count),
        "RaylibKeyMap is out of sync with KeyCode enum, update KeyCodeToRaylib when adding/removing keys."
    );

    // ===========================================================================
    // Reverse lookup  (O(1) sparse table, built at compile time)
    // ===========================================================================
    //
    // Raylib/GLFW key values range from 0 to 348 (KEY_KB_MENU).
    // A 512-element array covers the full range with a safe margin.
    //
    // Entries not corresponding to any mapped key hold KeyCode::Unknown (0).
    // ===========================================================================

    inline constexpr std::size_t RaylibReverseMapSize = 512;

    namespace detail {
        constexpr std::array<KeyCode, RaylibReverseMapSize> BuildReverseMap() noexcept
        {
            std::array<KeyCode, RaylibReverseMapSize> table{};
            // Zero-init fills every slot with KeyCode(0) == KeyCode::Unknown
            for (std::size_t i = 1; i < KeyCodeToRaylib.size(); ++i)
            {
                const int v = KeyCodeToRaylib[i];
                if (v > 0 && static_cast<std::size_t>(v) < RaylibReverseMapSize)
                    table[static_cast<std::size_t>(v)] = static_cast<KeyCode>(i);
            }
            return table;
        }
    } // namespace detail

    /// Sparse reverse table: RaylibReverseMap[raylibKey] -> KeyCode in O(1).
    inline constexpr auto RaylibReverseMap = detail::BuildReverseMap();

    /// Returns the KeyCode that maps to raylibKey in O(1).
    /// Returns KeyCode::Unknown if the value is not mapped or out of range.
    [[nodiscard]] inline constexpr KeyCode RaylibKeyToKeyCode(int raylibKey) noexcept
    {
        const auto idx = static_cast<std::size_t>(raylibKey);
        return (raylibKey > 0 && idx < RaylibReverseMapSize)
            ? RaylibReverseMap[idx]
            : KeyCode::Unknown;
    }

    // ===========================================================================
    // MouseButton -> Raylib mouse button integer mapping
    // ===========================================================================
    //
    // Raylib mouse button constants (raylib.h):
    //   MOUSE_BUTTON_LEFT   = 0
    //   MOUSE_BUTTON_RIGHT  = 1
    //   MOUSE_BUTTON_MIDDLE = 2
    //   MOUSE_BUTTON_SIDE   = 3
    //   MOUSE_BUTTON_EXTRA  = 4
    //   MOUSE_BUTTON_FORWARD  = 5
    //   MOUSE_BUTTON_BACK   = 6
    //
    // MouseButtonCount = 8 (Button0..Button7). Button7 has no Raylib equivalent
    // and maps to -1 (ignored).
    // ===========================================================================

    inline constexpr std::array<int, MouseButtonCount> MouseButtonToRaylib = {{
    //   Value   Index   MouseButton
         0,   //   0    Button0 / Left
         1,   //   1    Button1 / Right
         2,   //   2    Button2 / Middle
         3,   //   3    Button3 / Side
         4,   //   4    Button4 / Extra
         5,   //   5    Button5 / Forward
         6,   //   6    Button6 / Back
        -1,   //   7    Button7 (unmapped in Raylib)
    }};

    static_assert(
        MouseButtonToRaylib.size() == MouseButtonCount,
        "MouseButtonToRaylib is out of sync with MouseButtonCount."
    );

    /// Translates a MouseButton to the corresponding Raylib integer, or -1 if unmapped.
    [[nodiscard]] inline constexpr int ToRaylibMouseButton(MouseButton btn) noexcept
    {
        const auto idx = static_cast<std::size_t>(btn);
        return idx < MouseButtonToRaylib.size() ? MouseButtonToRaylib[idx] : -1;
    }

    // ===========================================================================
    // GamepadButton -> Raylib gamepad button integer mapping
    // ===========================================================================
    //
    // Raylib gamepad button constants (raylib.h):
    //   GAMEPAD_BUTTON_UNKNOWN          = 0
    //   GAMEPAD_BUTTON_LEFT_FACE_UP     = 1   -> GamepadButton::Up
    //   GAMEPAD_BUTTON_LEFT_FACE_RIGHT  = 2   -> GamepadButton::Right
    //   GAMEPAD_BUTTON_LEFT_FACE_DOWN   = 3   -> GamepadButton::Down
    //   GAMEPAD_BUTTON_LEFT_FACE_LEFT   = 4   -> GamepadButton::Left
    //   GAMEPAD_BUTTON_RIGHT_FACE_UP    = 5   -> GamepadButton::Y
    //   GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 6   -> GamepadButton::B
    //   GAMEPAD_BUTTON_RIGHT_FACE_DOWN  = 7   -> GamepadButton::A
    //   GAMEPAD_BUTTON_RIGHT_FACE_LEFT  = 8   -> GamepadButton::X
    //   GAMEPAD_BUTTON_LEFT_TRIGGER_1   = 9   -> GamepadButton::LB
    //   GAMEPAD_BUTTON_LEFT_TRIGGER_2   = 10  -> GamepadButton::LT
    //   GAMEPAD_BUTTON_RIGHT_TRIGGER_1  = 11  -> GamepadButton::RB
    //   GAMEPAD_BUTTON_RIGHT_TRIGGER_2  = 12  -> GamepadButton::RT
    //   GAMEPAD_BUTTON_MIDDLE_LEFT      = 13  -> GamepadButton::Back
    //   GAMEPAD_BUTTON_MIDDLE           = 14  -> GamepadButton::Guide
    //   GAMEPAD_BUTTON_MIDDLE_RIGHT     = 15  -> GamepadButton::Start
    //   GAMEPAD_BUTTON_LEFT_THUMB       = 16  -> GamepadButton::LeftStick
    //   GAMEPAD_BUTTON_RIGHT_THUMB      = 17  -> GamepadButton::RightStick
    //
    // GamepadButton::Unknown (0) has no Raylib equivalent (-1).
    // ===========================================================================

    // Number of GamepadButton entries including Unknown sentinel (0..RightStick).
    // Derived from the GamepadButton::Count sentinel to stay in sync automatically.
    inline constexpr std::size_t GamepadButtonCount = static_cast<std::size_t>(GamepadButton::Count);

    inline constexpr std::array<int, GamepadButtonCount> GamepadButtonToRaylib = {{
    //   Value   Index   GamepadButton
        -1,   //   0    Unknown           (no Raylib equivalent)
         1,   //   1    Up                GAMEPAD_BUTTON_LEFT_FACE_UP
         2,   //   2    Right             GAMEPAD_BUTTON_LEFT_FACE_RIGHT
         3,   //   3    Down              GAMEPAD_BUTTON_LEFT_FACE_DOWN
         4,   //   4    Left              GAMEPAD_BUTTON_LEFT_FACE_LEFT
         5,   //   5    Y                 GAMEPAD_BUTTON_RIGHT_FACE_UP
         6,   //   6    B                 GAMEPAD_BUTTON_RIGHT_FACE_RIGHT
         7,   //   7    A                 GAMEPAD_BUTTON_RIGHT_FACE_DOWN
         8,   //   8    X                 GAMEPAD_BUTTON_RIGHT_FACE_LEFT
         9,   //   9    LB                GAMEPAD_BUTTON_LEFT_TRIGGER_1
        10,   //  10    LT                GAMEPAD_BUTTON_LEFT_TRIGGER_2
        11,   //  11    RB                GAMEPAD_BUTTON_RIGHT_TRIGGER_1
        12,   //  12    RT                GAMEPAD_BUTTON_RIGHT_TRIGGER_2
        13,   //  13    Back              GAMEPAD_BUTTON_MIDDLE_LEFT
        14,   //  14    Guide             GAMEPAD_BUTTON_MIDDLE
        15,   //  15    Start             GAMEPAD_BUTTON_MIDDLE_RIGHT
        16,   //  16    LeftStick         GAMEPAD_BUTTON_LEFT_THUMB
        17,   //  17    RightStick        GAMEPAD_BUTTON_RIGHT_THUMB
    }};

    static_assert(
        GamepadButtonToRaylib.size() == GamepadButtonCount,
        "GamepadButtonToRaylib size mismatch, update when adding buttons."
    );

    /// Translates a GamepadButton to the corresponding Raylib integer, or -1 if unmapped.
    [[nodiscard]] inline constexpr int ToRaylibGamepadButton(GamepadButton btn) noexcept
    {
        const auto idx = static_cast<std::size_t>(btn);
        return idx < GamepadButtonToRaylib.size() ? GamepadButtonToRaylib[idx] : -1;
    }

    // ===========================================================================
    // GamepadAxis -> Raylib gamepad axis integer mapping
    // ===========================================================================
    //
    // Raylib gamepad axis constants (raylib.h):
    //   GAMEPAD_AXIS_LEFT_X        = 0
    //   GAMEPAD_AXIS_LEFT_Y        = 1
    //   GAMEPAD_AXIS_RIGHT_X       = 2
    //   GAMEPAD_AXIS_RIGHT_Y       = 3
    //   GAMEPAD_AXIS_LEFT_TRIGGER  = 4
    //   GAMEPAD_AXIS_RIGHT_TRIGGER = 5
    // ===========================================================================

    // Number of GamepadAxis entries (LeftX..RightTrigger).
    // Derived from the GamepadAxis::Count sentinel to stay in sync automatically.
    inline constexpr std::size_t GamepadAxisCount = static_cast<std::size_t>(GamepadAxis::Count);

    inline constexpr std::array<int, GamepadAxisCount> GamepadAxisToRaylib = {{
    //   Value   Index   GamepadAxis
         0,   //   0    LeftX             GAMEPAD_AXIS_LEFT_X
         1,   //   1    LeftY             GAMEPAD_AXIS_LEFT_Y
         2,   //   2    RightX            GAMEPAD_AXIS_RIGHT_X
         3,   //   3    RightY            GAMEPAD_AXIS_RIGHT_Y
         4,   //   4    LeftTrigger       GAMEPAD_AXIS_LEFT_TRIGGER
         5,   //   5    RightTrigger      GAMEPAD_AXIS_RIGHT_TRIGGER
    }};

    static_assert(
        GamepadAxisToRaylib.size() == GamepadAxisCount,
        "GamepadAxisToRaylib size mismatch, update when adding axes."
    );

    /// Translates a GamepadAxis to the corresponding Raylib integer.
    [[nodiscard]] inline constexpr int ToRaylibGamepadAxis(GamepadAxis axis) noexcept
    {
        const auto idx = static_cast<std::size_t>(axis);
        return idx < GamepadAxisToRaylib.size() ? GamepadAxisToRaylib[idx] : -1;
    }

} // namespace RS::Backend
