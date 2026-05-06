#pragma once

namespace RS {

class Window;

/**
 * @interface ImGuiBackend
 * @brief Abstract interface for Dear ImGui platform + renderer backends.
 *
 * Abstract interface defining the essential operations required by any Dear ImGui backend implementation.
 * It decouples ImGuiLayer from specific platform and renderer backends,
 * enabling flexible and extensible integration.
 */
class ImGuiBackend
{
public:
    virtual ~ImGuiBackend() = default;

    /**
     * @brief Initialize the platform and renderer backends.
     *
     * Called once after the ImGui context is created. Installs
     * event callbacks, loads OpenGL function pointers, etc.
     *
     * @param window The application's main window.
     */
    virtual void Init(Window& window) = 0;

    /**
     * @brief Shut down the platform and renderer backends.
     *
     * Called before the ImGui context is destroyed.
     * Must mirror what Init() set up.
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Start a new ImGui frame (platform + renderer side).
     *
     * Feeds platform-specific events (cursor position, keyboard state,
     * display size, etc.) into ImGuiIO. Must be called before
     * ImGui::NewFrame().
     */
    virtual void BeginFrame() = 0;

    /**
     * @brief Render ImGui draw data to the screen.
     *
     * Delegates to the renderer backend to render the ImGui draw lists.
     * Must be called after ImGui::Render().
     */
    virtual void EndFrame() = 0;
};

} // namespace RS
