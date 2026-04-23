#pragma once

#include <optional>
#include <memory>
#include <string>

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Math/Vector3.hpp"

#include "Raysim/Renderer/RendererAPI.hpp"
#include "Raysim/Renderer/RenderCommand.hpp"
#include "Raysim/Core/Window.hpp"
#include "Raysim/Input/Input.hpp"

#include "Raysim/Scene/SceneManager.hpp"

#include "Raysim/Math/Random.hpp"

#include "Raysim/Core/BackendFactory.hpp"
#include "Raysim/Core/EngineContext.hpp"
#include "Raysim/Core/ApplicationConfig.hpp"
#include "Raysim/Core/FontManager.hpp"
#include "Raysim/Core/Time.hpp"


// Exclude conflicting Win32 definitions that clash with raylib
// (wingdi.h defines Rectangle as a function, winuser.h defines CloseWindow/ShowCursor)
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef NOUSER
        #define NOUSER
    #endif
    #undef DrawTextEx
    #undef DrawText
#endif

namespace RS {

// ============================================================================
// Command-line arguments wrapper
// ============================================================================

#pragma region CommandLine Wrapper

/**
 * @struct ApplicationCommandLineArgs
 * @brief Simple wrapper for command-line arguments passed to the application.
 *
 * Provides convenient access to argc/argv parameters from main() and supports
 * range-based for loops for easy iteration over arguments.
 */
struct ApplicationCommandLineArgs {
    int Count = 0;           // Number of command-line arguments
    char** Args = nullptr;   // Pointer to argument array

    // Construct directly from main() parameters
    ApplicationCommandLineArgs() = default;
    ApplicationCommandLineArgs(int argc, char** argv)
        : Count(argc), Args(argv) {}

    // Bounds-checked argument access
    // Asserts in debug builds if index is out of range or Args is null
    const char* operator[](int index) const {
        RS_CORE_ASSERT(Args != nullptr, "Args pointer is null");
        RS_CORE_ASSERT(index >= 0 && index < Count, "Argument index {} out of range", index);
        return Args[index];
    }

    // Iterator support for range-based for loops
    char** begin() const { return Args; }
    char** end()   const { return Args + Count; }
};

#pragma endregion

// ============================================================================
// Main Application Class
// ============================================================================

#pragma region Application

/**
 * @class Application
 * @brief Base class for all Raysim applications.
 *
 * Inherit from this class and override the lifecycle methods to build
 * simulations, games, and interactive graphics apps.
 * Backend systems are accessed through the protected interface pointers
 * no Raylib headers needed in user code.
 */
class Application
{
public:
    /// Constructor: initializes application with specification
    Application(const ApplicationConfig& config = ApplicationConfig());

    /// Virtual destructor for proper cleanup in derived classes
    virtual ~Application() noexcept;

// ===========================================================================
// Operators
// ===========================================================================

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&)                 = default;
    Application& operator=(Application&&)      = default;

public:

// ============================================================================
// Scene management
// ============================================================================

    /// Register a new scene type with the SceneManager. This allows you to switch to this scene later by name or ID.
    template<typename T>
    void RegisterScene()
    {
        if (m_SceneManager) {
            m_SceneManager->RegisterScene<T>();
        } else {
            RS_CORE_ERROR("Cannot add scene: SceneManager is not initialized");
        }
    }

    /// Change to a different scene by specifying its type. The scene must have been registered previously.
    template<typename T, typename... Args>
    void ChangeScene(Args&&... args)
    {
        if (m_SceneManager) {
            m_SceneManager->ChangeScene<T>(std::forward<Args>(args)...);
        } else {
            RS_CORE_ERROR("Cannot change scene: SceneManager is not initialized");
        }
    }

    /// Set the initial scene to start with. This is a convenience method that simply changes to the specified scene at startup.
    template<typename T>
    void SetInitialScene()
    {
        if (m_SceneManager) {
            m_SceneManager->ChangeScene<T>();
        } else {
            RS_CORE_ERROR("Cannot set initial scene: SceneManager is not initialized");
        }
    }

// ===========================================================================
// Main loop
// ===========================================================================

    /// Start the main application loop. This will run until the window is closed or Quit() is called.
    void Run();

    /// Request application shutdown
    /// The application will finish the current frame before exiting
    void Close();

protected:

    // -- Resources -----------------------------------------------------------

    /// Load and register the default font for text rendering.
    /// @param fontPath  Path to a TTF/OTF font file.
    /// @param fontSize  Desired font size in pixels (default: 32).
    void SetDefaultFont(const std::string& fontPath, int fontSize = 32);

    // -- Misc ----------------------------------------------------------------

    /// Seed the global RNG for reproducible results.
    void SetRandomSeed(unsigned int seed);

private:

    ApplicationConfig   m_Configuration;  // Application configuration
    Scope<Window>       m_Window;         // The main window
    Shared<RendererAPI> m_Renderer;       // Renderer backend
    Scope<Input>        m_Input;          // Input backend

    Scope<RenderCommand> m_RenderCommand;

    std::optional<SceneManager> m_SceneManager;
    EngineContext m_EngineContext;

    /// Rebuild m_EngineContext from current backend pointers.
    void RebuildContext();

    bool m_Running   = true;                     // Application running state
    bool m_Minimized = false;                    // Is window minimized?
};

#pragma endregion

// ============================================================================
// Entry Point - TO BE IMPLEMENTED BY CLIENT
// ============================================================================

/**
 * @brief Client-defined function to create the application instance.
 *
 * Each client must implement this function to return a new instance of their
 * application class derived from Application. This is the entry point that the
 * platform layer will call to start the application.
 *
 * @param args Command-line arguments passed from main()
 * @return Pointer to a new Application instance
 */
Application* CreateApplication(ApplicationCommandLineArgs args);

} // namespace RS
