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
        static void Init(Scope<RendererAPI> api);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void ClearScreen(const Color &color);
        static void ClearScreen(const Math::Vec3f &color);
        static void Clear();

    private:
        // Defined in the .cpp to ensure a single shared instance across modules.
        // `inline static` would create separate copies in each module (EXE/DLL),
        // causing crashes in shared-library builds.
        static Scope<RendererAPI> m_API;
    };

} // namespace RS
