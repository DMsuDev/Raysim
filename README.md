<p align="center">
  <img src="docs/images/banner.png" alt="Raysim Banner" width="720" />
</p>

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Alpha-orange?style=flat)
[![Version](https://img.shields.io/badge/Version-0.93.0-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)
[![License](https://img.shields.io/badge/License-Apache%202.0-lightgrey?style=flat)](LICENSE)

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)
• [Changelog](docs/CHANGELOG.md)

Raysim is a C++ framework for 2D graphics and interactive applications, built on top of [raylib](https://www.raylib.com/).

Architecturally inspired by [**The Cherno's Hazel Engine**](https://github.com/TheCherno/Hazel), it provides a clean, class-based API for drawing shapes, handling input, managing time, and running fixed-timestep simulations.

Useful for learning graphics programming, prototyping ideas, or building small games and simulations.

Feedback and contributions are welcome.

> **Note:** project is in Alpha. The API may change without warning and some features are still being implemented.

## Quick Demos

<p align="center">
  <img src="docs/gif/Mouse2D.gif" alt="Mouse2D" width="480" /><br />
  <b>Mouse2D</b>
</p>

<p align="center">
  <img src="docs/gif/NoiseLandscape.gif" alt="NoiseLandscape" width="480" /><br />
  <b>NoiseLandscape</b>
</p>

<p align="center">
  <img src="docs/gif/SceneShowcase.gif" alt="SceneShowcase" width="480" /><br />
  <b>SceneShowcase</b>
</p>

| Example           | Description                                                                                                                                                                                       |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Physics simulation with gravity, mouse attraction/repulsion, and ball spawning. Spawn balls with left click, toggle gravity with G, and watch them collide and bounce against the walls.          |
| `LissajousCurves` | Parametric curve visualiser with animated phase shift and selectable frequency presets. The curve evolves in real time; try switching presets with the ImGui panel to see the patterns transform. |
| `Mouse2D`         | Tracks the mouse position with smooth exponential interpolation and draws a trailing circle. A good starting point to understand the input and rendering pipeline.                                |
| `NoiseLandscape`  | Procedurally generated scrolling terrain rendered in layered passes using Perlin, Simplex, Cellular, and Value noise. Press R to reseed all layers and get a completely new landscape.            |
| `SceneShowcase`   | Three independent visual scenes (StarField, PlasmaArt, ClockMandala) accessible from a shared ImGui navigation overlay. Shows how to switch scenes at runtime using the SceneManager.             |

All examples are built alongside the library. See the [Building](#building) section for setup instructions.

## Application Loop

Each application cycle runs through the active scene's lifecycle callbacks in order. Override only the ones you need.

| Callback                 | When it is called                                                                                                          |
| ------------------------ | -------------------------------------------------------------------------------------------------------------------------- |
| `OnAttach`               | Once, when the scene is first pushed. Load assets, push layers, subscribe to the EventBus.                                 |
| `OnStart`                | Every time the scene becomes active (after `OnAttach` or when resumed). Reset simulation state.                            |
| `OnUpdate(dt)`           | Every frame. Input polling, game logic, variable-rate state updates.                                                       |
| `OnFixedUpdate(fixedDt)` | At a fixed timestep, independent of frame rate. Physics integration and deterministic simulation steps.                    |
| `OnDraw(alpha)`          | Every frame after `OnUpdate`. All rendering commands go here. `alpha` is a value in [0, 1) used for physics interpolation. |
| `OnPause`                | When the scene is paused by the SceneManager.                                                                              |
| `OnResume`               | When the scene is resumed from a paused state.                                                                             |
| `OnDetach`               | When the scene is removed. Release resources and unsubscribe from the EventBus.                                            |

`OnDraw` receives an interpolation factor `alpha` so you can lerp between the previous and current physics snapshot for smooth rendering at any frame rate. Putting all rendering inside `OnUpdate` instead is also valid; `alpha` will simply be 0.

## Modules

<details>
<summary>Core</summary>

| Symbol              | Purpose                                                                                                                                                              |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Root class. Owns the main loop, backend, `SceneManager`, and `EngineContext`. Register scenes with `RegisterScene<T>()` and activate them with `ChangeScene<T>()`.   |
| `ApplicationConfig` | Declarative configuration (window title, width, height, log level, max fixed steps). Pass to the `Application` constructor; all fields have sensible defaults.       |
| `EngineContext`     | Plain struct passed to every scene. Holds non-owning pointers to `Window`, `Input`, `SceneManager`, `ApplicationConfig`, and `EventBus`.                             |
| `Time`              | Static utility: delta time, fixed timestep, time scale, pause/resume, smoothed FPS, and `GetRealtimeSinceStartup()`.                                                 |
| `Log`               | Thin wrapper around **spdlog**. Writes to console and an optional log file. Macros: `RS_LOG_TRACE`, `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`, `RS_LOG_CRITICAL`. |
| `FontManager`       | Load a TTF/OTF font once with a key and reference it globally for text rendering. The `"default"` key is used when no key is specified.                              |
| `BackendFactory`    | Creates concrete `RendererAPI`, `Window`, and `Input` instances for the active backend.                                                                              |

</details>

<details>
<summary>Scene System</summary>

Organises the application into independent, self-contained screens (main menu, gameplay, pause screen, etc.), each with its own lifecycle, layer stack, and subsystem access.

| Symbol         | Purpose                                                                                                           |
| -------------- | ----------------------------------------------------------------------------------------------------------------- |
| `Scene`        | Base class. Override lifecycle callbacks. Use `RS_SCENE(MyScene)` to register the type automatically.             |
| `SceneManager` | Manages the scene stack. Supports `ChangeScene`, `PushScene`, `PopScene`, pause/resume, and lookup by ID or name. |

</details>

<details>
<summary>Events</summary>

Two complementary event mechanisms are provided:

Layer-based propagation: events dispatched by the `Window` are forwarded through the `LayerStack` in reverse order (top-most layer first). A layer can stop propagation by setting `e.Handled = true`. Ideal for UI overlays that should absorb input before it reaches game layers.

EventBus: a type-safe publish/subscribe broadcast channel owned by the `Application` and exposed through `EngineContext::Bus`. All subscribers receive the event in subscription order regardless of the `Handled` flag. Useful for decoupled systems such as audio, achievements, or debug hotkeys. Always subscribe in `OnAttach` and unsubscribe in `OnDetach`.

| Built-in event category | Events                                                                                         |
| ----------------------- | ---------------------------------------------------------------------------------------------- |
| Window                  | `WindowCloseEvent`, `WindowResizeEvent`, `WindowFocusEvent`                                    |
| Keyboard                | `KeyPressedEvent`, `KeyReleasedEvent`, `KeyTypedEvent`                                         |
| Mouse                   | `MouseButtonPressedEvent`, `MouseButtonReleasedEvent`, `MouseMovedEvent`, `MouseScrolledEvent` |

</details>

<details>
<summary>Layers</summary>

A `Layer` is a modular unit inside a scene's `LayerStack`. It receives the same callbacks as a scene plus `OnUIRender()` for ImGui widgets. The stack is divided into two groups: layers (processed bottom-to-top) and overlays (always processed after all layers). Add them from inside `OnAttach` with `PushLayer` / `PushOverlay`.

| Method             | Effect                                     |
| ------------------ | ------------------------------------------ |
| `PushLayer(layer)` | Inserts a layer and calls `OnAttach()`.    |
| `PushOverlay(ov)`  | Adds an overlay and calls `OnAttach()`.    |
| `PopLayer(layer)`  | Removes a layer and calls `OnDetach()`.    |
| `PopOverlay(ov)`   | Removes an overlay and calls `OnDetach()`. |

</details>

<details>
<summary>Graphics</summary>

| Symbol       | Purpose                                                                                                                                                                                                     |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Shapes`     | Filled and outlined primitives: rectangles, circles, lines, triangles. Each function accepts an optional `OriginMode` to anchor the shape at its center or any corner/edge instead of the default top-left. |
| `Texts`      | Draw text strings at a screen position using the currently loaded font.                                                                                                                                     |
| `Color`      | RGBA color struct `{r, g, b, a}` with `Lerp`, `FromHSV`, and operator overloads.                                                                                                                            |
| `Colors`     | `constexpr` preset namespace: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayWhite`, and more.                                                                           |
| `OriginMode` | Enum controlling the anchor point of a `Shapes` primitive (`TopLeft`, `Center`, `BottomRight`, etc.).                                                                                                       |

</details>

<details>
<summary>Math</summary>

All math types live in the `RS::Math` namespace.

| Symbol         | Purpose                                                                                                                                                                                                    |
| -------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Vec2f`        | `Vector2<float>`. 2D float vector with arithmetic operators, `Length`, `Normalize`, `Distance`, `Dot`, `Reflect`, `Project`, and `RotatedBy(Angle)`.                                                       |
| `Vec2i`        | `Vector2<int>`. Integer 2D vector with the same operator set.                                                                                                                                              |
| `Vec3f`        | `Vector3<float>`. 3D float vector, used internally and for general math.                                                                                                                                   |
| `Vec3i`        | `Vector3<int>`. Integer 3D vector.                                                                                                                                                                         |
| `Angle`        | Strongly-typed angle wrapper stored internally in radians. Construct via `Angle::Degrees(90.0f)`, `Angle::Radians(x)`, or the `90.0_deg` user-defined literal. Prevents accidental mixing of units.        |
| `Math` (utils) | `Lerp`, `InverseLerp`, `Remap`, `Clamp`, `Clamp01`, `SmoothStep`, `SmoothDamp`, `MoveTowards`, `PingPong`, `Repeat`, `EpsilonEquals`, `Saturate`, `Fract`, and more.                                       |
| `Random`       | Seeded Mersenne Twister RNG. Integer/float ranges, booleans, container sampling. Coherent noise: Perlin 2D/3D, Simplex, Cellular, Value, and FBM. Seed is auto-random on startup via `std::random_device`. |

</details>

<details>
<summary>Input</summary>

The `Input` interface provides backend-agnostic access to keyboard, mouse, and gamepad state. Access it with `GetInput()` from any scene or layer.

| Query category | Key methods                                                                                     |
| -------------- | ----------------------------------------------------------------------------------------------- |
| Keyboard       | `IsKeyDown`, `IsKeyPressed`, `IsKeyReleased`, `IsKeyRepeating`                                  |
| Mouse          | `IsMouseButtonDown/Pressed/Released`, `GetMousePosition`, `GetMouseDelta`, `GetMouseWheelDelta` |
| Gamepad        | `IsGamepadConnected`, `IsGamepadButtonDown/Pressed`, `GetGamepadAxis`                           |

Gamepad buttons and axes use the `GamepadButton` and `GamepadAxis` enums with an Xbox-style layout: A/B/X/Y face buttons, LB/RB shoulder buttons, LT/RT triggers, D-Pad, and thumb-sticks.

Event-driven backends (GLFW, SDL2, etc.) extend `EventDrivenInput` instead of `Input` directly. `EventDrivenInput` maintains per-frame Down/Pressed/Released/Repeating state arrays populated through `OnEvent()` and cleared each `Update()`.

</details>

<details>
<summary>ImGui Integration</summary>

`ImGuiLayer` is built in for debug overlays and in-app UI panels. Push it as an overlay in `OnAttach()`, then implement `OnUIRender()` on any `Layer` subclass to draw ImGui widgets. The context is initialised and shut down automatically.

**SceneShowcase** demonstrates a shared `ImGui` overlay (`SceneNavLayer`) that switches between three sub-scenes at runtime.

</details>

<details>
<summary>Interfaces and Backend</summary>

Five abstract interfaces decouple user code from the underlying graphics library:

| Interface      | Responsibility                                           |
| -------------- | -------------------------------------------------------- |
| `RendererAPI`  | Frame begin/end, screen clear, VSync control.            |
| `Window`       | Title, size, fullscreen toggle, aspect ratio.            |
| `Input`        | Keyboard, mouse, and gamepad state.                      |
| `ImGuiBackend` | ImGui initialisation, shutdown, and per-frame lifecycle. |
| `FontRenderer` | Font loading, glyph measurement, and text rendering.     |

The `Raylib` backend is the only implementation currently included. `RaylibRendererAPI`, `RaylibWindow`, `RaylibInput`, `RaylibImGuiBackend`, and `RaylibFontRenderer` satisfy each interface. All backend-specific headers are confined to this layer and never leak into user code.

The active backend is selected at configure time via the `RS_BACKEND` CMake cache option (`raylib` by default).

</details>

## Building

Minimum requirements: CMake 3.28, C++20, and Ninja. Dependencies are managed via [vcpkg](https://vcpkg.io/), which is included as a Git submodule.

macOS support has not been officially tested. The build system and dependencies should work in theory, but compatibility is not guaranteed.

### 1. Clone with submodules

vcpkg is included as a Git submodule, so you need to initialise it after cloning. If you cloned the repo without `--recurse-submodules`, run:

```bash
git submodule update --init --recursive
```

### 2. Setup

Once the submodule is ready, the setup script bootstraps vcpkg and installs all required dependencies automatically:

```bash
./tools/setup_all.sh       # Linux / macOS
.\tools\setup_all.ps1      # Windows (PowerShell)
```

### 3. Build

Using CMake presets is the recommended and quickest way to build:

```bash
cmake --preset debug              # Configure Debug (Ninja)
cmake --build --preset debug      # Build Debug

cmake --preset release            # Configure Release (Ninja)
cmake --build --preset release    # Build Release
```

The `debug` preset enables ASan and UBSan by default. Sanitizer support is experimental and may not work on all toolchains; on MinGW they are automatically disabled.

### Manual CMake

When not using presets, pass the vcpkg toolchain file manually so CMake can locate the installed packages.

#### With Ninja (recommended, all platforms)

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DRS_BUILD_EXAMPLES=ON
cmake --build build
```

#### With Visual Studio (Windows only)

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DRS_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

Visual Studio is a multi-config generator. It ignores `-DCMAKE_BUILD_TYPE` at configure time and requires `--config` at build time instead.

## Quick Start

```cpp
#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MyScene : public Scene {
    RS_SCENE(MyScene)

private:
    Math::Vec2f position = {400.0f, 300.0f};
    Math::Vec2f velocity = {150.0f, 100.0f};

    void OnFixedUpdate(float fixedDt) override {
        position += velocity * fixedDt;

        float width  = static_cast<float>(GetWindow().GetWidth());
        float height = static_cast<float>(GetWindow().GetHeight());

        if (position.x < 20 || position.x > width  - 20) velocity.x *= -1;
        if (position.y < 20 || position.y > height - 20) velocity.y *= -1;
    }

    void OnDraw(float /*alpha*/) override {
        GetRenderer().ClearScreen(Colors::DarkBlue);
        Shapes::DrawCircle(position.x, position.y, 20.0f, Colors::RayWhite);
    }
};

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "My First Scene";
    config.Window.Width  = 800;
    config.Window.Height = 600;

    auto* app = new RS::Application(config);
    app->RegisterScene<MyScene>();
    app->ChangeScene<MyScene>();
    return app;
}
```

The seed is auto-random on startup. Call `Math::Random::Seed(value)` in `OnAttach()` only if you need a reproducible sequence.

## License

This project is licensed under the **Apache License 2.0**.
See the [LICENSE](LICENSE) file for details.

<p align="center">
  <img src="docs/images/footer.png" alt="Raysim Footer" width="600" />
</p>
