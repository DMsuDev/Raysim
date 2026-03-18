#pragma once

namespace RS {

struct Color;
struct Vector3;

/**
 * @interface IRenderer
 * @brief Abstract interface for rendering operations.
 *
 * Responsible for frame presentation only. Frame-rate capping is handled
 * by the Application loop, not the renderer, so that every backend can
 * implement its own sleeping strategy transparently.
 */
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    // -- Frame ---------------------------------------------------------------

    /// Called at the start of each frame to prepare the renderer state.
    virtual void BeginFrame() = 0;

    /// Called at the end of each frame to present the result to the screen.
    virtual void EndFrame() = 0;

    // -- Clear screen --------------------------------------------------------

    /// Clear the render target with a Color.
    virtual void ClearScreen(const Color& color) = 0;

    /// Clear the render target with normalised RGB values (0.0 – 1.0).
    virtual void ClearScreen(const Vector3& color) = 0;

    // -- V-Sync --------------------------------------------------------------

    /**
     * @brief Enable or disable vertical synchronisation.
     * When enabled, presentation waits for the monitor refresh to avoid tearing.
     */
    virtual void SetVSync(bool enabled) = 0;

    /// Returns true if VSync is currently enabled.
    virtual bool IsVSync() const = 0;
};

} // namespace RS
