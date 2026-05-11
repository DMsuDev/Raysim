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

/**
 * @enum KeyCode
 * @brief Backend‑agnostic keyboard key codes.
 *
 * Defines a layout‑independent set of key identifiers used internally by the input system.
 * Values are sequential and zero‑based to allow efficient indexing.
 *
 * These codes do not correspond to any platform API. Each backend maps its native
 * key values to this enum through a dedicated translation layer.
 */
enum class KeyCode : uint16_t
{
    Unknown = 0, ///< Unknown / unmapped key

    // -- Printable keys (layout-agnostic) -----------------------------------------------
    Space,
    Apostrophe,
    Comma,
    Minus,
    Period,
    Slash,

    // -- Digits (top row) ----------------------------------------------------------------
    D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,

    Semicolon,
    Equal,

    // -- Alphabet ------------------------------------------------------------------------
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    LeftBracket,
    Backslash,
    RightBracket,
    GraveAccent,

    // -- Control and navigation --------------------------------------------------------
    Escape, Enter, Tab, Backspace,
    Insert, Delete,
    Right, Left, Down, Up,
    PageUp, PageDown, Home, End,

    // -- State keys --------------------------------------------------------------------
    CapsLock, ScrollLock, NumLock, PrintScreen, Pause,

    // -- Function keys -----------------------------------------------------------------
    F1,  F2,  F3,  F4,  F5,  F6,
    F7,  F8,  F9,  F10, F11, F12,

    // -- Numpad ------------------------------------------------------------------------
    KP0, KP1, KP2, KP3, KP4,
    KP5, KP6, KP7, KP8, KP9,
    KPDecimal, KPDivide, KPMultiply,
    KPSubtract, KPAdd, KPEnter, KPEqual,

    // -- Modifier keys -----------------------------------------------------------------
    LeftShift,  LeftControl,  LeftAlt,  LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper,

    Menu,

    Count   ///< Sentinel: number of valid key codes (do not use as a key)
};

// ============================================================================
// Scan codes
// ============================================================================

/// Physical key identifier independent of keyboard layout.
///
/// The concrete value is backend-defined:
///   - Raylib backend : GLFW physical key code (layout-independent).
///   - Other backends : OS hardware scancode or equivalent.
using ScanCode = int32_t;
inline constexpr ScanCode InvalidScanCode = -1;

// ============================================================================
// Modifier flags
// ============================================================================

/**
 * @enum Modifier
 * @brief Bitmask flags representing keyboard modifier keys (Shift, Control, Alt, Super).
 *
 * Defines a set of modifier keys used by the input system. Values are
 * represented as individual bit flags so they can be combined using
 * bitwise operations.
 *
 * These flags do not correspond to any platform API. Each backend maps
 * its native modifier state to this enum through a translation layer.
 */
enum class Modifier : uint8_t
{
    None    = 0,
    Shift   = 1 << 0,
    Control = 1 << 1,
    Alt     = 1 << 2,
    Super   = 1 << 3,
};

/**
 * @brief Bitwise OR operator for Modifier enum to allow combining flags into a bitmask.
 *
 * Enables syntax like `Modifier::Shift | Modifier::Control` to create a combined bitmask of active modifiers.
 *
 * @param a First Modifier operand.
 * @param b Second Modifier operand.
 * @return A new Modifier value representing the combination of the input flags.
 */

inline Modifier operator| (Modifier a, Modifier b) noexcept { return static_cast<Modifier>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
/**
 * @brief Compound assignment operator for Modifier enum to allow adding flags to an existing bitmask.
 *
 * This operator allows syntax like `mods |= Modifier::Alt` to add a flag to an existing bitmask.
 *
 * @param a Modifier variable to modify (left-hand side).
 * @param b Modifier flag to add (right-hand side).
 * @return A reference to the modified Modifier variable after adding the flag.
 */
inline Modifier& operator|=(Modifier& a, Modifier b) noexcept { return a = a | b; }

/**
 * @brief Bitwise AND operator for Modifier enum to allow checking for specific flags in a bitmask.
 *
 * This operator enables syntax like `if (mods & Modifier::Control) { ... }` to check if a specific modifier is active.
 *
 * @param a First Modifier operand (bitmask).
 * @param b Second Modifier operand (flag to check).
 * @return A new Modifier value where the specified flag is set if it was present in the input bitmask; otherwise, None.
 */
inline Modifier operator& (Modifier a, Modifier b) noexcept { return static_cast<Modifier>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }

/**
 * @brief Checks whether a specific modifier flag is present in a bitmask.
 *
 * @param flags Combined Modifier bitmask to check.
 * @param mod Modifier flag to test (e.g. Modifier::Shift).
 * @return true if the specified modifier is active within the flags; false otherwise.
 */
inline bool HasModifier(Modifier flags, Modifier mod) noexcept { return (flags & mod) != Modifier::None; }

} // namespace RS
