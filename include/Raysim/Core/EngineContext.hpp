#pragma once

namespace RS
{
    // Forward declarations of core engine subsystems to avoid circular dependencies

    // --- Core system headers ---
    struct ApplicationConfig;

    // --- Subsystem headers ---
    class Window;
    class SceneManager;
    class RenderCommand;
    class Input;
    class EventBus;

    /**
     * @struct EngineContext
     * @brief A simple struct to hold pointers to core engine subsystems.
     *
     * This context is passed to scenes and other systems that need access to core functionality
     * without directly depending on the Application class. It provides a clean interface to
     * access the window, input system, renderer, and scene manager from anywhere in the codebase.
     */
    struct EngineContext
    {
        ApplicationConfig *Config = nullptr; // Configuration settings
        SceneManager *Scenes = nullptr;      // Scene management and transitions
        Window *MainWindow = nullptr;        // Main window
        Input *InputSystem = nullptr;        // Input polling
        EventBus *Bus = nullptr;             // Global pub/sub event bus
    };

} // namespace RS
