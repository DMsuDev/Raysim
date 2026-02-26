#pragma once

#include "Raysim/Manager/FontManager.hpp"
#include "Raysim/Manager/TimeManager.hpp"
#include "Raysim/Input/InputManager.hpp"

namespace RS {

/**
 * @class Canvas
 * @brief Main application class for RaySim simulations and graphics applications
 *
 * Canvas is the primary interface users interact with to build applications with RaySim.
 * It manages the window, rendering context, input handling, and timing systems.
 *
 * Key Features:
 *
 * - Virtual Methods: Override Setup() for initialization and Draw() for frame rendering
 *
 * - Integrated Managers: Built-in InputManager, TimeManager, and FontManager
 *
 * - Window Control: Manage window size, title, FPS, and fullscreen states
 *
 * - Automatic Updates: Managers are updated automatically in the Run() loop
 */
class Canvas
{
public:
    /**
     * @brief Construct a new Canvas object
     *
     * Creates a Canvas with default settings: 800×600 window and 60 FPS target.
     */
    Canvas() = default;

    /**
     * @brief Destroy the Canvas object
     *
     * Virtual destructor to allow proper cleanup in derived classes.
     */
    virtual ~Canvas() noexcept = default;

    // Delete copy constructor and assignment operator to prevent copying

    Canvas(const Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;

    // Allow move semantics for flexibility

    Canvas(Canvas&&) = default;
    Canvas& operator=(Canvas&&) = default;

    /**
     * @brief User-defined initialization (called once at application start)
     *
     * Override this method to set up resources, initialize state, and configure the window.
     * Called automatically before the application loop begins.
     *
     * @example
     * void Setup() override {
     *     SetTitle("My Game");
     *     SetSize(1920, 1080);
     *     SetTargetFPS(144);
     *     SetDefaultFont("resources/font.ttf", 24);
     * }
     */
    virtual void Setup() {}

    /**
     * @brief User-defined frame rendering (called every frame)
     *
     * Override this method to implement your simulation logic and rendering.
     * Called repeatedly within the application loop after updates but before rendering completes.
     *
     * @example
     * void Draw() override {
     *     Renderer::Background(Color::Black());
     *
     *     auto& input = GetInputManager();
     *     auto& time = GetTimeManager();
     *
     *     if (time.IsInterval(0.5f)) {
     *         SpawnEnemy();
     *     }
     *
     *     Renderer::DrawText("FPS: " + std::to_string(time.GetCurrentFPS()),
     *                        400, 20, 16, Color::White(), OriginMode::TopCenter);
     * }
     */
    virtual void Draw() {}

    /**
     * @brief User-defined update logic (called every frame before Draw())
     *
     * Override this method to implement non-rendering logic that should run every frame, such as game state updates,
     * physics calculations, or AI behavior.
     *
     * @note This method is optional for logic that doesn't need to run every frame or can be handled within Draw(), but it provides a clear separation of concerns for update logic.
     */
    virtual void Update() {}

    /**
     * @brief Start the main application loop
     *
     * Initializes the windowing and rendering systems, calls `Setup()` once, then
     * repeatedly calls `Update()` and `Draw()` until the application exits. This
     * method drives the integrated managers (input, time, font) and blocks until
     * the window is closed.
     *
     * @note Initializes logging and the underlying raylib context before calling `Setup()`.
     * @see Setup(), Update(), Draw()
     */
    void Run();

    // ============================================================================
    // WINDOW MANAGEMENT
    // ============================================================================

    /**
     * @brief Set the window title
     *
     * Updates the visible title for the application window.
     *
     * @param title The display name for the window (UTF-8 string)
     */
    void SetTitle(const std::string& title);

    /**
     * @brief Set the target frames-per-second for the main loop
     *
     * Instructs the internal `TimeManager` to limit the application update/render
     * rate to the provided value.
     *
     * @param fps Desired frames per second (must be > 0)
     * @see TimeManager::SetTargetFPS()
     */
    void SetTargetFPS(int fps);

    /**
     * @brief Set the window size in pixels
     *
     * Resizes the application window to the specified width and height. Behavior
     * may depend on the current fullscreen/windowed mode and underlying platform.
     *
     * @param width Window width in pixels (> 0)
     * @param height Window height in pixels (> 0)
     */
    void SetSize(int width, int height);

    /**
     * @brief Toggle fullscreen mode
     *
     * Switches the window between windowed and fullscreen states. The exact
     * fullscreen behavior (borderless, exclusive) depends on the platform/renderer.
     */
    void ToggleFullscreen();

    // ============================================================================
    // WINDOW PROPERTIES
    // ============================================================================

    /// @brief Get the current window dimensions as a Vector2
    Vector2 GetScreenSize() const { return {static_cast<float>(width_), static_cast<float>(height_)}; }

    /// @brief Get window width in pixels
    int GetWidth() const { return width_; }

    /// @brief Get window height in pixels
    int GetHeight() const { return height_; }

    // ============================================================================
    // MANAGER ACCESS
    // ============================================================================

    /**
     * @brief Set the default font used for text rendering
     *
     * Loads and registers a default font for the built-in rendering/text helpers.
     * If loading fails, the font manager may keep a fallback font.
     *
     * @param fontPath Filesystem path to a font file (TTF/OTF)
     * @param fontSize Desired font size in pixels (defaults to 32)
     */
    void SetDefaultFont(const std::string& fontPath, int fontSize = 32);

    /**
     * @brief Access the font manager
     *
     * Returns a reference to the internal `FontManager` used to load and cache fonts.
     *
     * @return Reference to the `FontManager` instance owned by this Canvas
     */
    FontManager& GetFontManager() { return font_; }

    /**
     * @brief Get the input manager
     *
     * Provides access to the `InputManager` that collects keyboard, mouse, and
     * gamepad state each frame.
     *
     * @return Reference to the `InputManager` instance
     */
    InputManager& GetInputManager() { return input_; }

    /**
     * @brief Get const reference to the input manager
     *
     * @return Const reference to the `InputManager` instance
     */
    const InputManager& GetInputManager() const { return input_; }

    /**
     * @brief Get the time manager
     *
     * Returns the `TimeManager` responsible for tracking delta time, FPS and
     * providing timing utilities to the application.
     *
     * @return Reference to the `TimeManager` instance
     */
    TimeManager& GetTimeManager() { return time_; }

    /**
     * @brief Get const reference to the time manager
     *
     * @return Const reference to the `TimeManager` instance
     */
    const TimeManager& GetTimeManager() const { return time_; }

    // ============================================================================
    // RANDOM SEED
    // ============================================================================

    /**
     * @brief Set the seed for random number generation
     * @param seed Integer seed value for reproducible randomness
     * @see Math::Random::SetSeed()
     */
    void SetRandomSeed(unsigned int seed) { Random::Seed(seed); }

    // ============================================================================
    // RENDERING
    // ============================================================================

    /**
     * @brief Clear the screen using a solid color
     *
     * Clears the current render target and fills the entire viewport with the
     * provided color. Call this at the beginning of a frame before other draw
     * operations to establish a background.
     *
     * @param color Color used to clear the screen
     * @example
     * Renderer::Background(Color::Black());  // Clear to black
     */
    void Background(const RS::Color& color);

private:
    bool isRunning_ { false };        ///< Flag indicating if the main loop is running

    int fps_               {60};      ///< Target frames per second
    int width_             {800};     ///< Window width in pixels
    int height_            {600};     ///< Window height in pixels
    std::string title_     {"RaySim Title"};  ///< Window title

    // Integrated managers
    FontManager font_;         ///< Font loading and management
    InputManager input_;       ///< Keyboard, mouse, gamepad input
    TimeManager time_;         ///< FPS control and frame timing
};

} // namespace RS
