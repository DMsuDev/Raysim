#pragma once

#include "Raysim/Core/Base.hpp"

#include "Raysim/Math/Types/Vec2.hpp"
#include "Raysim/Math/Types/Vec3.hpp"
#include "Raysim/Graphics/Color.hpp"

#include <cstdint>

namespace RS {

/**
 * @interface RendererAPI
 * @brief Abstract interface for low-level rendering operations.
 *
 * Defines the core rendering operations that must be implemented by each
 * graphics backend (e.g. OpenGL, DirectX, Vulkan). The Renderer class
 * delegates to a concrete RendererAPI instance to perform actual rendering.
 *
 * @see Renderer
 */
class RendererAPI
{
public:
    virtual ~RendererAPI() = default;

// ====================================================================
// Initialization
// ====================================================================

    /**
     * @brief Initialize the rendering API.
     *
     * Must be called **once** after construction and before any rendering
     * command is issued. Implementations should:
     * - Load extension function pointers (e.g. via glad for OpenGL).
     * - Configure default render states (depth test, blending, etc.).
     * - Query and log GPU/driver information.
     */
    virtual void Init() {}

// ====================================================================
// Frame management
// ====================================================================

    /**
     * @brief Begin a new render frame.
     *
     * Called at the start of each frame before any draw calls. Implementations
     * may perform per-frame setup here, such as clearing command buffers or
     * resetting internal state.
     */
    virtual void BeginFrame() = 0;

    /**
     * @brief End the current render frame and present the result.
     *
     * Called at the end of each frame after all draw calls. Implementations
     * should flush any pending commands and present the rendered image to
     * the screen (e.g. by swapping buffers).
     */
    virtual void EndFrame() = 0;

// ====================================================================
// Viewport
// ====================================================================

    /**
     * @brief Set the rendering viewport region.
     *
     * Defines the rectangle of the framebuffer that will be rendered into.
     * All subsequent draw calls are clipped to this region. Coordinates are
     * in framebuffer pixels (not logical / DPI-scaled units).
     *
     * @param x      Left edge of the viewport (pixels, framebuffer origin).
     * @param y      Bottom edge of the viewport (pixels, framebuffer origin).
     * @param width  Width of the viewport in pixels.  Must be > 0.
     * @param height Height of the viewport in pixels. Must be > 0.
     */
    virtual void SetViewport(uint32_t x, uint32_t y,
                             uint32_t width, uint32_t height) = 0;

// ====================================================================
// Screen clear
// ====================================================================

    /**
     * @brief Clear the colour buffer to the given RGBA colour.
     *
     * @param color The clear colour as an RGBA `Color` value.
     *              Alpha handling is backend-dependent for the default
     *              framebuffer; it is respected for off-screen targets.
     */
    virtual void ClearScreen(const Color& color)   = 0;

    /**
     * @brief Clear the colour buffer using a linear-colour RGB triplet.
     *
     * Each component should be in [0, 1]. Values outside this range produce
     * implementation-defined results. Alpha is implicitly set to 1.0.
     *
     * @param color RGB colour vector (linear space).
     */
    virtual void ClearScreen(const Math::Vec3f& color) = 0;

    /**
     * @brief Clear all buffers (color, depth, stencil) to their default values.
     *
     * This is a convenience method that typically calls `ClearScreen` with the
     * default clear color and also clears the depth and stencil buffers.
     */
    virtual void Clear() = 0;
};

} // namespace RS
