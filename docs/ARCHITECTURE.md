# Architecture

This document covers the internal structure of Raysim: the application lifecycle, the module overview, and how the pieces fit together.

---

## Application Loop

Each frame the engine walks the active scene's lifecycle callbacks in a fixed order. Override only the ones you need — unimplemented callbacks are no-ops.

| Callback                 | When it is called                                                                                                      |
| ------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| `OnAttach`               | Once, when the scene is first pushed onto the stack. Load assets, push layers, subscribe to the EventBus.              |
| `OnStart`                | Every time the scene becomes active (after `OnAttach` or when resumed from pause). Reset simulation state here.        |
| `OnUpdate(dt)`           | Every frame. Input polling, game logic, variable-rate state updates.                                                   |
| `OnFixedUpdate(fixedDt)` | At a fixed timestep, independent of frame rate. Physics integration and deterministic simulation steps belong here.    |
| `OnDraw(alpha)`          | Every frame, after `OnUpdate`. All rendering commands go here. `alpha` is a value in [0, 1) for physics interpolation. |
| `OnPause`                | When the scene is paused by the `SceneManager`.                                                                        |
| `OnResume`               | When the scene is resumed from a paused state.                                                                         |
| `OnDetach`               | When the scene is removed from the stack. Release resources and unsubscribe from the EventBus.                         |

> [!TIP]
> `OnDraw` receives an interpolation factor `alpha` so you can lerp between the previous and current physics snapshot for smooth rendering at any frame rate. If you put all rendering inside `OnUpdate` instead, `alpha` will simply be 0 — both approaches are valid.

<!-- -->

> [!CAUTION]
> Never allocate or load assets inside `OnDraw`. Use `OnAttach` or `OnStart` for one-time setup. `OnDraw` is called every frame and allocation there will cause hitches.

---

## Modules

### Core

| Symbol              | Purpose                                                                                                                                                              |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Root class. Owns the main loop, backend, `SceneManager`, and `EngineContext`. Register scenes with `RegisterScene<T>()` and activate them with `ChangeScene<T>()`.   |
| `ApplicationConfig` | Declarative configuration (window title, width, height, log level, max fixed steps). Pass to the `Application` constructor; all fields have sensible defaults.       |
| `EngineContext`     | Plain struct passed to every scene and layer. Holds non-owning pointers to `Window`, `Input`, `SceneManager`, `ApplicationConfig`, and `EventBus`.                   |
| `Time`              | Static utility: delta time, fixed timestep, time scale, pause/resume, smoothed FPS, and `GetRealtimeSinceStartup()`.                                                 |
| `Log`               | Thin wrapper around **spdlog**. Writes to console and an optional log file. Macros: `RS_LOG_TRACE`, `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`, `RS_LOG_CRITICAL`. |
| `FontManager`       | Load a TTF/OTF font once with a key and reference it globally for text rendering. The `"default"` key is used when no explicit key is specified.                     |
| `BackendFactory`    | Creates concrete `RendererAPI`, `Window`, and `Input` instances for the active backend selected at compile time.                                                     |

> [!NOTE]
> `RS_LOG_*` macros are zero-cost in Release builds when the log level is above the threshold — the expression is never evaluated. Use them freely without performance concern.

---

### Scene System

Organises the application into independent, self-contained screens (main menu, gameplay, pause screen, etc.), each with its own lifecycle, layer stack, and subsystem access.

| Symbol         | Purpose                                                                                                           |
| -------------- | ----------------------------------------------------------------------------------------------------------------- |
| `Scene`        | Base class. Override lifecycle callbacks. Use `RS_SCENE(MyScene)` to register the type automatically.             |
| `SceneManager` | Manages the scene stack. Supports `ChangeScene`, `PushScene`, `PopScene`, pause/resume, and lookup by ID or name. |

`ChangeScene<T>()` replaces the current scene immediately. `PushScene<T>()` suspends (but keeps) the current scene and pushes a new one on top — useful for pause menus or sub-flows that need to return to the previous screen.

---

### Events

Two complementary mechanisms are provided. Use the one that matches your architectural need — they are not mutually exclusive.

**Layer-based propagation (blocking, ordered)**
Events dispatched by the `Window` are forwarded through the `LayerStack` from top to bottom. A layer stops propagation by setting `e.Handled = true`. Ideal for UI overlays that must absorb input before the game layer sees it.

**EventBus (broadcast, unordered)**
A type-safe publish/subscribe channel owned by the `Application` and exposed through `EngineContext::Bus`. All subscribers receive the event regardless of the `Handled` flag. Useful for decoupled systems: audio, achievements, debug hotkeys.

> [!WARNING]
> Always subscribe in `OnAttach` and **unsubscribe in `OnDetach`**. The `EventBus` outlives any individual layer. Failing to unsubscribe leaves a dangling callback that will be called after the layer is destroyed.

| Built-in event category | Events                                                                                         |
| ----------------------- | ---------------------------------------------------------------------------------------------- |
| Window                  | `WindowCloseEvent`, `WindowResizeEvent`, `WindowFocusEvent`                                    |
| Keyboard                | `KeyPressedEvent`, `KeyReleasedEvent`, `KeyTypedEvent`                                         |
| Mouse                   | `MouseButtonPressedEvent`, `MouseButtonReleasedEvent`, `MouseMovedEvent`, `MouseScrolledEvent` |

---

### Layers

A `Layer` is a modular unit inside a scene's `LayerStack`. It receives the same lifecycle callbacks as a scene, plus `OnUIRender()` for Dear ImGui widgets. The stack has two groups: layers (processed bottom-to-top for rendering, top-to-bottom for events) and overlays (always on top of all layers).

| Method             | Effect                                     |
| ------------------ | ------------------------------------------ |
| `PushLayer(layer)` | Inserts a layer and calls `OnAttach()`.    |
| `PushOverlay(ov)`  | Adds an overlay and calls `OnAttach()`.    |
| `PopLayer(layer)`  | Removes a layer and calls `OnDetach()`.    |
| `PopOverlay(ov)`   | Removes an overlay and calls `OnDetach()`. |

---

### Graphics

| Symbol       | Purpose                                                                                                                                                                                                     |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Shapes`     | Filled and outlined primitives: rectangles, circles, lines, triangles. Each function accepts an optional `OriginMode` to anchor the shape at its center or any corner/edge instead of the default top-left. |
| `Texts`      | Draw text strings at a screen position using a loaded font handle.                                                                                                                                          |
| `Color`      | RGBA color struct `{r, g, b, a}` with `Lerp`, `FromHSV`, and operator overloads.                                                                                                                            |
| `Colors`     | `constexpr` preset namespace: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayWhite`, and more.                                                                           |
| `OriginMode` | Enum controlling the anchor point of a `Shapes` primitive (`TopLeft`, `Center`, `BottomRight`, etc.).                                                                                                       |

---

### Math

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

> [!TIP]
> Call `Math::Random::Seed(value)` in `OnAttach()` only if you need a reproducible sequence (e.g. for testing or replay). The default auto-seed is sufficient for most use cases.

---

### Input

The `Input` interface provides backend-agnostic access to keyboard, mouse, and gamepad state. Access it via `GetInput()` from any scene or layer.

| Query category | Key methods                                                                                     |
| -------------- | ----------------------------------------------------------------------------------------------- |
| Keyboard       | `IsKeyDown`, `IsKeyPressed`, `IsKeyReleased`, `IsKeyRepeating`                                  |
| Mouse          | `IsMouseButtonDown/Pressed/Released`, `GetMousePosition`, `GetMouseDelta`, `GetMouseWheelDelta` |
| Gamepad        | `IsGamepadConnected`, `IsGamepadButtonDown/Pressed`, `GetGamepadAxis`                           |

Gamepad buttons and axes use the `GamepadButton` and `GamepadAxis` enums with an Xbox-style layout: A/B/X/Y face buttons, LB/RB shoulder buttons, LT/RT triggers, D-Pad, and thumb-sticks.

Event-driven backends (GLFW, SDL2, etc.) extend `EventDrivenInput` instead of `Input` directly. It maintains per-frame Down/Pressed/Released/Repeating state arrays populated via `OnEvent()` and cleared each `Update()`.

---

### ImGui Integration

`ImGuiLayer` is built in for debug overlays and in-app UI panels. Push it as an overlay in `OnAttach()`, then implement `OnUIRender()` on any `Layer` subclass to draw Dear ImGui widgets. The context is initialised and shut down automatically.

> [!IMPORTANT]
> All Dear ImGui calls must be placed inside `OnUIRender()`. Calling ImGui functions in `OnDraw` or `OnUpdate` will produce no output or trigger assertion failures inside the ImGui library.

**SceneShowcase** demonstrates a shared ImGui overlay (`SceneNavLayer`) that switches between three sub-scenes at runtime — a good reference for multi-scene UI.

---

### Interfaces and Backend

Five abstract interfaces decouple user code from the underlying graphics library:

| Interface      | Responsibility                                           |
| -------------- | -------------------------------------------------------- |
| `RendererAPI`  | Frame begin/end, screen clear, VSync control.            |
| `Window`       | Title, size, fullscreen toggle, aspect ratio.            |
| `Input`        | Keyboard, mouse, and gamepad state.                      |
| `ImGuiBackend` | ImGui initialisation, shutdown, and per-frame lifecycle. |
| `FontRenderer` | Font loading, glyph measurement, and text rendering.     |

The `Raylib` backend is the only implementation currently included. `RaylibRendererAPI`, `RaylibWindow`, `RaylibInput`, `RaylibImGuiBackend`, and `RaylibFontRenderer` satisfy each interface. All backend-specific headers are confined to this layer and never leak into user code.

The active backend is selected at configure time via the `RS_BACKEND` CMake cache option (`raylib` by default). See [BACKENDS.md](BACKENDS.md) for instructions on implementing a custom backend.

---

<p align="center">
  <img src="images/footer.png" alt="Raysim Footer" width="720" />
</p>
