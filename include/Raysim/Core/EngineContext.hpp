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
