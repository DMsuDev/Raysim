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
     * @brief Start the main application loop
     *
     * Initializes the window, calls Setup() once, then repeatedly calls Draw() and Update()
     * until the window is closed. Handles all manager updates automatically.
     * Blocks until the application terminates.
     *
     * @note This method initializes logging and raylib
     * @see Setup(), Draw()
     */
    void Run();

    // ============================================================================
    // WINDOW MANAGEMENT
    // ============================================================================

    /**
     * @brief Set the window title
     * @param title Display name for the window
     */
    void SetTitle(const std::string& title);

    /**
     * @brief Set target frame rate
     * @param fps Frames per second (e.g., 60, 120, 144)
     * @see TimeManager::SetTargetFPS()
     */
    void SetTargetFPS(int fps);

    /**
     * @brief Set window dimensions
     * @param width Width in pixels
     * @param height Height in pixels
     */
    void SetSize(int width, int height);

    /**
     * @brief Toggle between windowed and fullscreen modes
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
     * @brief Set the default font for text rendering
     * @param fontPath Path to font file (.ttf, .otf, etc.)
     * @param fontSize Font size in pixels
     */
    void SetDefaultFont(const std::string& fontPath, int fontSize = 32);

    /**
     * @brief Get the font manager
     * @return Reference to the FontManager instance
     */
    FontManager& GetFontManager() { return fontManager_; }

    /**
     * @brief Get the input manager for keyboard, mouse, and gamepad input
     * @return Reference to the InputManager instance
     */
    InputManager& GetInputManager() { return inputManager_; }

    /// @brief Get const reference to InputManager
    const InputManager& GetInputManager() const { return inputManager_; }

    /**
     * @brief Get the time manager for FPS control and frame timing
     * @return Reference to the TimeManager instance
     */
    TimeManager& GetTimeManager() { return timeManager_; }

    /// @brief Get const reference to TimeManager
    const TimeManager& GetTimeManager() const { return timeManager_; }

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
     * @brief Fill the entire screen with a solid color
     *
     * Clears the screen and fills it with the specified color.
     * Typically called at the start of each frame before drawing other elements.
     *
     * @param color The fill color
     * @example
     * Renderer::Background(Color::Black());  // Black background
     */
    void Background(const RS::Color& color);

private:
    // Window configuration

    int fps_               {60};      ///< Target frames per second
    int width_             {800};     ///< Window width in pixels
    int height_            {600};     ///< Window height in pixels
    std::string title_     {"RaySim Title"};  ///< Window title

    // Integrated managers
    FontManager fontManager_;         ///< Font loading and management
    InputManager inputManager_;       ///< Keyboard, mouse, gamepad input
    TimeManager timeManager_;         ///< FPS control and frame timing
};

} // namespace RS
