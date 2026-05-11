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
 * @enum MouseButton
 * @brief Mouse button identifiers used across the input system.
 *
 * Values are sequential and have no relation to any underlying API's numeric codes.
 * Additional semantic aliases (Left, Right, Middle) are provided for common buttons.
 *
 * The `MouseButton::Last` enumerator serves as a sentinel to determine the total count of buttons.
 */
enum class MouseButton : uint16_t
{
	Button0 = 0,
	Button1 = 1,
	Button2 = 2,
	Button3 = 3,
	Button4 = 4,
	Button5 = 5,
	Button6 = 6,
	Button7 = 7,

	// Semantically meaningful aliases for common buttons

	Left = Button0,    ///< Left mouse button
	Right = Button1,   ///< Right mouse button
	Middle = Button2,  ///< Middle mouse button (wheel click)

	Last = Button7     ///< Sentinel value representing the last valid button index.
};

/**
 * @brief Total number of mouse buttons defined in the MouseButton enum.
 * This can be used for iterating over all buttons or validating button indices.
 */
constexpr std::size_t MouseButtonCount = static_cast<std::size_t>(MouseButton::Last) + 1u;

/**
 * @enum CursorMode
 * @brief Cursor visibility and behavior modes for interactive contexts.
 *
 * - Normal: visible and free to move.
 * - Hidden: invisible but still reports movement.
 * - Locked: hidden and constrained to window center (typical for FPS cameras).
 */
enum class CursorMode : uint8_t
{
	Normal = 0,  ///< Cursor is visible and moves freely.
	Hidden = 1,  ///< Cursor is invisible but moves freely.
	Locked = 2,  ///< Cursor is hidden and locked to the window center (FPS camera).
};

} // namespace RS
