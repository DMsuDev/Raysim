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

#include "Raysim/Renderer/RendererAPI.hpp"

namespace RS
{
    /**
     * @class RenderCommand
     * @brief Thin command dispatcher that forwards rendering calls to the active RendererAPI.
     *
     * Provides a convenient facade so that user code can issue render commands
     * without a direct dependency on any concrete graphics API.
     * The RendererAPI instance is owned by RenderCommand and is initialized via Init().
     *
     * @see RendererAPI
     */
    class RenderCommand
    {
    public:

        /// @brief Initializes the RenderCommand system with a specific RendererAPI backend.
        /// Transfers ownership of the backend and calls its internal initialization routine.
        /// Must be called once at application startup before issuing any render commands.
        /// @param api A Scope<RendererAPI> instance representing the graphics API to use.
        static void Init(Scope<RendererAPI> api);

        /// @brief Shuts down the RenderCommand system.
        /// Releases the active RendererAPI instance and clears all renderer state.
        static void Shutdown();

        /// @brief Marks the beginning of a new rendering frame.
        /// Prepares the active RendererAPI for issuing draw commands.
        static void BeginFrame();

        /// @brief Finalizes the current rendering frame.
        /// Flushes pending commands and presents the rendered output.
        static void EndFrame();

        /// @brief Sets the viewport dimensions for rendering.
        /// The viewport defines the area of the render target that will be drawn to.
        /// @param x The x-coordinate of the viewport's lower-left corner.
        /// @param y The y-coordinate of the viewport's lower-left corner.
        /// @param width The width of the viewport.
        /// @param height The height of the viewport.
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        /// @brief Clears the screen with the given color.
        /// @param color The color to clear the color buffer with, specified as an RGBA Color value.
        static void ClearScreen(const Color &color);

        /// @brief Clears the screen using the default clear color (black).
        /// Useful for passes where a custom clear color is not required.
        static void Clear();

    private:
        // Defined in the .cpp to guarantee a single shared instance across all modules.
        // Using `inline static` would create one instance per translation unit,
        // which breaks shared-library builds (EXE + DLL) and leads to undefined behavior.
        static Scope<RendererAPI> m_API;
    };

} // namespace RS
