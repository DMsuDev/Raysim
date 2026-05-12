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

// Exclude conflicting Win32 definitions that clash with raylib
// (wingdi.h defines Rectangle as a function, winuser.h defines CloseWindow/ShowCursor)
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
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
    #undef CreateWindow
#endif

// --- Properties and enums ---
#include "Raysim/Core/ApplicationConfig.hpp"
#include "Raysim/Core/EngineContext.hpp"

#include "Raysim/Scene/SceneManager.hpp"

namespace RS {

// ============================================================================
// Forward declarations
// ============================================================================

class Window;
class Input;
class Event;
class EventBus;

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

// ===========================================================================
// Main loop
// ===========================================================================

    /// Start the main application loop. This will run until the window is closed or Quit() is called.
    void Run();

    /// Request application shutdown
    /// The application will finish the current frame before exiting
    void Close();

protected:

private:

    ApplicationConfig   m_Configuration;  // Application configuration
    Scope<Window>       m_Window;         // The main window
    Scope<Input>        m_Input;          // Input backend
    Scope<EventBus>     m_EventBus;       // Global pub/sub event bus

    Scope<SceneManager> m_SceneManager;
    EngineContext m_EngineContext;

    // Rebuild m_EngineContext from current backend pointers.
    void RebuildContext();

    // Engine-level event handler called for every event the Window dispatches.
    // Handles WindowClose / WindowResize, then forwards to the SceneManager.
    void OnEvent(Event& e);

    bool m_Running   = true;  // Application running state
    bool m_Minimized = false; // Is window minimized?

    // Max fixed update steps per frame (prevents spiral of death)
    uint32_t m_MaxFixedSteps = 8;
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
