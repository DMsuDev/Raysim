#pragma once

#include <memory>
#include <string>

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Math/Vector3.hpp"

#include "Raysim/Interfaces/IRenderer.hpp"
#include "Raysim/Interfaces/IWindow.hpp"
#include "Raysim/Interfaces/IInput.hpp"
#

#include "Raysim/Math/Random.hpp"

#include "ApplicationConfig.hpp"
#include "BackendFactory.hpp"
#include "FontManager.hpp"
#include "Time.hpp"

namespace RS {

/**
 * @class Application
 * @brief Base class for all Raysim applications.
 *
 * Inherit from this class and override the lifecycle methods to build
 * simulations, games, and interactive graphics apps.
 * Backend systems are accessed through the protected interface pointers—
 * no Raylib headers needed in user code.
 */
class Application
{
public:
    explicit Application(const ApplicationConfig& config = {});
    virtual ~Application() noexcept = default;

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&)                 = default;
    Application& operator=(Application&&)      = default;

    /// Initialises the backend, opens the window and runs the main loop.
    void Run();

protected:
    // -- Backend interfaces --------------------------------------------------

    std::unique_ptr<IRenderer> Renderer;
    std::unique_ptr<IWindow>   Window;
    std::unique_ptr<IInput>    Input;

    // -- Lifecycle -----------------------------------------------------------

    /// Called once before the main loop. Use it to configure the window, load assets, etc.
    virtual void Setup() {}

    /// Called once per frame for non-rendering logic (input, physics, AI, ...).
    /// @param dt Scaled delta time for this frame (seconds). Same as Time::GetDeltaTime().
    virtual void Update(float dt) { (void)dt; }

    /// Called at a fixed timestep (configured via Time::SetFixedDeltaTime()).
    /// Prefer this over Update() for deterministic physics simulations.
    /// @param fixedDt Fixed scaled timestep (seconds). Same as Time::GetFixedDeltaTime().
    virtual void FixedUpdate(float fixedDt) { (void)fixedDt; }

    /// Called once per frame after Update(). Issue all draw commands here.
    /// @param alpha Interpolation factor in [0, 1) for blending physics snapshots. Same as Time::GetInterpolationAlpha().
    /// Avoid mutating game state inside Draw().
    virtual void Draw(float alpha) = 0;

    // -- Window --------------------------------------------------------------

    void    SetTitle(const std::string& title);
    void    SetSize(int width, int height);
    void    ToggleFullscreen();
    bool    IsFullscreen()   const;
    int     GetWidth()       const;
    int     GetHeight()      const;
    float   GetAspectRatio() const;
    Vector2 GetWindowSize()  const;

    // -- Rendering -----------------------------------------------------------

    /// Clear the screen with a solid colour. Call at the start of Draw().
    void Background(const Color& color);

    // -- Resources -----------------------------------------------------------

    /// Load and register the default font for text rendering.
    /// @param fontPath  Path to a TTF/OTF font file.
    /// @param fontSize  Desired font size in pixels (default: 32).
    void SetDefaultFont(const std::string& fontPath, int fontSize = 32);

    // -- Misc ----------------------------------------------------------------

    /// Seed the global RNG for reproducible results.
    void SetRandomSeed(unsigned int seed);

    // -- Simulation control --------------------------------------------------

    /// Pauses the simulation (delegates to Time::Pause()).
    void Pause()   { Time::Pause(); }

    /// Resumes the simulation (delegates to Time::Resume()).
    void Resume()  { Time::Resume(); }

    /// Toggles pause/resume state.
    void TogglePause() { Time::IsPaused() ? Time::Resume() : Time::Pause(); }

    /// Returns true if the simulation is currently paused.
    bool IsPaused() const { return Time::IsPaused(); }

    /// Requests the application to stop after the current frame.
    void Quit() { isRunning_ = false; }

private:
    void RunInternal();
    void Shutdown();

    ApplicationConfig config_;
    BackendType       backendType_ = BackendType::Raylib;
    bool              isRunning_   = false;
};

} // namespace RS
