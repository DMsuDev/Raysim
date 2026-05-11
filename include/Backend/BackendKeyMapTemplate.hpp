/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 **********************************************************************************************/

// ===========================================================================
//  BACKEND KEYMAP TEMPLATE
//  Copy this file into the backend folder and rename it (e.g. SDLKeyMap.hpp).
//  Replace every TODO comment and every placeholder value with the real
//  constants from your backend's own header (SDL_SCANCODE_*, VK_*, etc.).
//
//  Steps:
//    1. Replace the namespace alias  RS::Backend  ->  RS::Backend (keep it).
//    2. Fill in KeyCodeToXxx   - one integer per RS KeyCode.
//    3. Fill in MouseButtonToXxx - one integer per RS MouseButton.
//    4. Fill in GamepadButtonToXxx - one integer per RS GamepadButton.
//    5. Fill in GamepadAxisToXxx   - one integer per RS GamepadAxis.
//    6. Adjust the ReverseMapSize constant to cover your backend's key range.
//    7. Delete all TODO comments when done.
//    8. Keep the static_asserts - they catch sync errors at compile time.
// ===========================================================================

#pragma once

#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Input/MouseCodes.hpp"
#include "Raysim/Input/GamepadCodes.hpp"

#include <array>

namespace RS::Backend
{
    // ===========================================================================
    // Section 1: KeyCode -> Native key integer
    // ===========================================================================
    //
    // NOTES:
    //   - Index 0 is KeyCode::Unknown - always map it to 0 (or the native "no key").
    //   - Use 0 for keys your backend does not support; they will be silently ignored.
    //   - The ordering MUST match the KeyCode enum declared in KeyCodes.hpp exactly.
    //   - Add a comment per line showing the index, KeyCode name, and native constant.
    //
    // TODO: Replace "XxxKey" with the name of your backend (e.g. "SDLKey", "Win32Key").
    // TODO: Replace every 0 placeholder after index 0 with the real native value.
    // ===========================================================================

    inline constexpr std::array<int, static_cast<std::size_t>(KeyCode::Count)> KeyCodeToXxx = {{
    //   Value   Index   KeyCode            Native constant
         0,   //   0    Unknown            (none)
         0,   //   1    Space              TODO: BACKEND_SPACE
         0,   //   2    Apostrophe         TODO
         0,   //   3    Comma              TODO
         0,   //   4    Minus              TODO
         0,   //   5    Period             TODO
         0,   //   6    Slash              TODO
         0,   //   7    D0                 TODO
         0,   //   8    D1                 TODO
         0,   //   9    D2                 TODO
         0,   //  10    D3                 TODO
         0,   //  11    D4                 TODO
         0,   //  12    D5                 TODO
         0,   //  13    D6                 TODO
         0,   //  14    D7                 TODO
         0,   //  15    D8                 TODO
         0,   //  16    D9                 TODO
         0,   //  17    Semicolon          TODO
         0,   //  18    Equal              TODO
         0,   //  19    A                  TODO
         0,   //  20    B                  TODO
         0,   //  21    C                  TODO
         0,   //  22    D                  TODO
         0,   //  23    E                  TODO
         0,   //  24    F                  TODO
         0,   //  25    G                  TODO
         0,   //  26    H                  TODO
         0,   //  27    I                  TODO
         0,   //  28    J                  TODO
         0,   //  29    K                  TODO
         0,   //  30    L                  TODO
         0,   //  31    M                  TODO
         0,   //  32    N                  TODO
         0,   //  33    O                  TODO
         0,   //  34    P                  TODO
         0,   //  35    Q                  TODO
         0,   //  36    R                  TODO
         0,   //  37    S                  TODO
         0,   //  38    T                  TODO
         0,   //  39    U                  TODO
         0,   //  40    V                  TODO
         0,   //  41    W                  TODO
         0,   //  42    X                  TODO
         0,   //  43    Y                  TODO
         0,   //  44    Z                  TODO
         0,   //  45    LeftBracket        TODO
         0,   //  46    Backslash          TODO
         0,   //  47    RightBracket       TODO
         0,   //  48    GraveAccent        TODO
         0,   //  49    Escape             TODO
         0,   //  50    Enter              TODO
         0,   //  51    Tab                TODO
         0,   //  52    Backspace          TODO
         0,   //  53    Insert             TODO
         0,   //  54    Delete             TODO
         0,   //  55    Right              TODO
         0,   //  56    Left               TODO
         0,   //  57    Down               TODO
         0,   //  58    Up                 TODO
         0,   //  59    PageUp             TODO
         0,   //  60    PageDown           TODO
         0,   //  61    Home               TODO
         0,   //  62    End                TODO
         0,   //  63    CapsLock           TODO
         0,   //  64    ScrollLock         TODO
         0,   //  65    NumLock            TODO
         0,   //  66    PrintScreen        TODO
         0,   //  67    Pause              TODO
         0,   //  68    F1                 TODO
         0,   //  69    F2                 TODO
         0,   //  70    F3                 TODO
         0,   //  71    F4                 TODO
         0,   //  72    F5                 TODO
         0,   //  73    F6                 TODO
         0,   //  74    F7                 TODO
         0,   //  75    F8                 TODO
         0,   //  76    F9                 TODO
         0,   //  77    F10                TODO
         0,   //  78    F11                TODO
         0,   //  79    F12                TODO
         0,   //  80    KP0                TODO
         0,   //  81    KP1                TODO
         0,   //  82    KP2                TODO
         0,   //  83    KP3                TODO
         0,   //  84    KP4                TODO
         0,   //  85    KP5                TODO
         0,   //  86    KP6                TODO
         0,   //  87    KP7                TODO
         0,   //  88    KP8                TODO
         0,   //  89    KP9                TODO
         0,   //  90    KPDecimal          TODO
         0,   //  91    KPDivide           TODO
         0,   //  92    KPMultiply         TODO
         0,   //  93    KPSubtract         TODO
         0,   //  94    KPAdd              TODO
         0,   //  95    KPEnter            TODO
         0,   //  96    KPEqual            TODO
         0,   //  97    LeftShift          TODO
         0,   //  98    LeftControl        TODO
         0,   //  99    LeftAlt            TODO
         0,   // 100    LeftSuper          TODO
         0,   // 101    RightShift         TODO
         0,   // 102    RightControl       TODO
         0,   // 103    RightAlt           TODO
         0,   // 104    RightSuper         TODO
         0,   // 105    Menu               TODO
    }};

    static_assert(
        KeyCodeToXxx.size() == static_cast<std::size_t>(KeyCode::Count),
        "KeyCodeToXxx is out of sync with KeyCode enum."
    );

    // ---------------------------------------------------------------------------
    // Reverse lookup (O(1) sparse table, built at compile time)
    // ---------------------------------------------------------------------------
    //
    // Set XxxReverseMapSize to (max native key value + 1) with a small margin.
    // Example: GLFW max = 348, so 512 is enough. SDL2 scancodes go up to ~284.
    //
    // TODO: Adjust XxxReverseMapSize to match your backend's key range.
    // ---------------------------------------------------------------------------

    inline constexpr std::size_t XxxReverseMapSize = 512; // TODO: adjust

    namespace detail_xxx {
        constexpr std::array<KeyCode, XxxReverseMapSize> BuildXxxReverseMap() noexcept
        {
            std::array<KeyCode, XxxReverseMapSize> table{};
            for (std::size_t i = 1; i < KeyCodeToXxx.size(); ++i)
            {
                const int v = KeyCodeToXxx[i];
                if (v > 0 && static_cast<std::size_t>(v) < XxxReverseMapSize)
                    table[static_cast<std::size_t>(v)] = static_cast<KeyCode>(i);
            }
            return table;
        }
    } // namespace detail_xxx

    /// Sparse reverse table: XxxReverseMap[nativeKey] -> KeyCode in O(1).
    inline constexpr auto XxxReverseMap = detail_xxx::BuildXxxReverseMap();

    /// Returns the KeyCode for the given native key integer, or KeyCode::Unknown.
    [[nodiscard]] inline constexpr KeyCode XxxKeyToKeyCode(int nativeKey) noexcept
    {
        const auto idx = static_cast<std::size_t>(nativeKey);
        return (nativeKey > 0 && idx < XxxReverseMapSize)
            ? XxxReverseMap[idx]
            : KeyCode::Unknown;
    }

    // ===========================================================================
    // Section 2: MouseButton -> Native mouse button integer
    // ===========================================================================
    //
    // Use -1 for buttons your backend does not support.
    // MouseButtonCount is defined in MouseCodes.hpp (currently 8).
    //
    // TODO: Replace "XxxMouseButton" names and values with your backend's constants.
    // ===========================================================================

    inline constexpr std::array<int, MouseButtonCount> MouseButtonToXxx = {{
    //   Value   Index   MouseButton
        -1,   //   0    Button0 / Left     TODO: BACKEND_MOUSE_LEFT
        -1,   //   1    Button1 / Right    TODO: BACKEND_MOUSE_RIGHT
        -1,   //   2    Button2 / Middle   TODO: BACKEND_MOUSE_MIDDLE
        -1,   //   3    Button3            TODO or -1 if unsupported
        -1,   //   4    Button4            TODO or -1 if unsupported
        -1,   //   5    Button5            TODO or -1 if unsupported
        -1,   //   6    Button6            TODO or -1 if unsupported
        -1,   //   7    Button7            TODO or -1 if unsupported
    }};

    static_assert(
        MouseButtonToXxx.size() == MouseButtonCount,
        "MouseButtonToXxx is out of sync with MouseButtonCount."
    );

    /// Translates a MouseButton to the native integer, or -1 if unmapped.
    [[nodiscard]] inline constexpr int ToXxxMouseButton(MouseButton btn) noexcept
    {
        const auto idx = static_cast<std::size_t>(btn);
        return idx < MouseButtonToXxx.size() ? MouseButtonToXxx[idx] : -1;
    }

    // ===========================================================================
    // Section 3: GamepadButton -> Native gamepad button integer
    // ===========================================================================
    //
    // GamepadButton::Unknown (index 0) should always map to -1.
    // Use -1 for buttons your backend does not support.
    // The array size must equal GamepadButtonCount (derived from GamepadButton::Count).
    // GamepadButton::Count is kept in sync with the enum automatically.
    //
    // TODO: Replace values with your backend's gamepad button constants.
    // ===========================================================================

    inline constexpr std::size_t XxxGamepadButtonCount = static_cast<std::size_t>(GamepadButton::Count);

    inline constexpr std::array<int, XxxGamepadButtonCount> GamepadButtonToXxx = {{
    //   Value   Index   GamepadButton
        -1,   //   0    Unknown
        -1,   //   1    Up                 TODO
        -1,   //   2    Right              TODO
        -1,   //   3    Down               TODO
        -1,   //   4    Left               TODO
        -1,   //   5    Y                  TODO
        -1,   //   6    B                  TODO
        -1,   //   7    A                  TODO
        -1,   //   8    X                  TODO
        -1,   //   9    LB                 TODO
        -1,   //  10    LT                 TODO
        -1,   //  11    RB                 TODO
        -1,   //  12    RT                 TODO
        -1,   //  13    Back               TODO
        -1,   //  14    Guide              TODO
        -1,   //  15    Start              TODO
        -1,   //  16    LeftStick          TODO
        -1,   //  17    RightStick         TODO
    }};

    static_assert(
        GamepadButtonToXxx.size() == XxxGamepadButtonCount,
        "GamepadButtonToXxx size mismatch."
    );

    /// Translates a GamepadButton to the native integer, or -1 if unmapped.
    [[nodiscard]] inline constexpr int ToXxxGamepadButton(GamepadButton btn) noexcept
    {
        const auto idx = static_cast<std::size_t>(btn);
        return idx < GamepadButtonToXxx.size() ? GamepadButtonToXxx[idx] : -1;
    }

    // ===========================================================================
    // Section 4: GamepadAxis -> Native gamepad axis integer
    // ===========================================================================
    //
    // The array size must equal XxxGamepadAxisCount (derived from GamepadAxis::Count).
    //
    // TODO: Replace values with your backend's axis constants.
    // ===========================================================================

    inline constexpr std::size_t XxxGamepadAxisCount = static_cast<std::size_t>(GamepadAxis::Count);

    inline constexpr std::array<int, XxxGamepadAxisCount> GamepadAxisToXxx = {{
    //   Value   Index   GamepadAxis
        -1,   //   0    LeftX              TODO
        -1,   //   1    LeftY              TODO
        -1,   //   2    RightX             TODO
        -1,   //   3    RightY             TODO
        -1,   //   4    LeftTrigger        TODO
        -1,   //   5    RightTrigger       TODO
    }};

    static_assert(
        GamepadAxisToXxx.size() == XxxGamepadAxisCount,
        "GamepadAxisToXxx size mismatch."
    );

    /// Translates a GamepadAxis to the native integer, or -1 if unmapped.
    [[nodiscard]] inline constexpr int ToXxxGamepadAxis(GamepadAxis axis) noexcept
    {
        const auto idx = static_cast<std::size_t>(axis);
        return idx < GamepadAxisToXxx.size() ? GamepadAxisToXxx[idx] : -1;
    }

} // namespace RS::Backend
