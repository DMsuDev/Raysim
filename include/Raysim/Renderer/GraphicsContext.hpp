#pragma once

#include "Raysim/Core/Base.hpp"

namespace RS {

/**
 * @class GraphicsContext
 * @brief Abstract interface for managing a low-level graphics API context.
 *
 * A GraphicsContext encapsulates the API-specific state that is required to
 * issue rendering commands and present frames to the screen. Responsibilities
 * include loading function pointers (e.g. glad for OpenGL), creating API
 * objects (swap-chains, command queues, etc.), and buffer presentation.
 *
 * **This class is an internal implementation detail of RendererAPI backends
 * that need explicit context management** (e.g. OpenGL, Vulkan). Backends
 * like Raylib handle context management internally and therefore do not
 * create a GraphicsContext.
 *
 * Concrete RendererAPI implementations that require a context should own a
 * `Scope<GraphicsContext>` and drive it from their own Init/EndFrame methods:
 *
 * @code
 * // Inside OpenGLRendererAPI
 * void Init() override {
 *     m_Context = GraphicsContext::Create(nativeWindow);
 *     m_Context->Init();
 * }
 * void EndFrame() override {
 *     // ... flush draw calls ...
 *     m_Context->SwapBuffers();
 * }
 * @endcode
 *
 * @see RendererAPI
 */
class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;

    /**
     * @brief Initialize the graphics context.
     *
     * Must be called once after construction and before issuing any draw calls.
     * Implementations load API function pointers, query renderer/driver info,
     * and set initial API state.
     */
    virtual void Init() = 0;

    /**
     * @brief Present the back buffer and prepare for the next frame.
     *
     * Swaps the front and back buffers (double-buffering). Called by the
     * owning RendererAPI at the end of each frame after all rendering
     * commands have been submitted.
     */
    virtual void SwapBuffers() = 0;

    /**
     * @brief Factory method to create a GraphicsContext for the specified native window.
     *
     * The concrete type is determined by the renderer API currently in use
     * (e.g. an OpenGL backend will create an OpenGL context).
     *
     * @param window Native window handle (e.g. `GLFWwindow*` for OpenGL).
     * @return Owning `Scope<GraphicsContext>` to the newly created context.
     */
    static Scope<GraphicsContext> Create(void* window);
};

} // namespace RS
