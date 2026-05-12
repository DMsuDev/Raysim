# Examples

This document provides practical, in-depth examples for every module in the Raysim framework. Each section explains the underlying mechanism first, then shows the corresponding code. Sections build on each other — if a concept (e.g. `EngineContext`, `EventBus`) is used without explanation, refer to the earlier section that introduced it.

> **Navigation**: [Home](../README.md) • [Backends](./BACKENDS.md) • Examples

---

## Table of Contents

- [Examples](#examples)
  - [Table of Contents](#table-of-contents)
  - [Quick Reference: Minimal Scene](#quick-reference-minimal-scene)
  - [Core](#core)
    - [Application Configuration](#application-configuration)
    - [EngineContext Access](#enginecontext-access)
    - [Time Utilities](#time-utilities)
    - [Logging](#logging)
    - [Font Loading and Rendering](#font-loading-and-rendering)
  - [Scene System](#scene-system)
    - [Defining a Scene with RS_SCENE](#defining-a-scene-with-rs_scene)
    - [Scene Lifecycle Walkthrough](#scene-lifecycle-walkthrough)
    - [Scene Transitions (ChangeScene, PushScene, PopScene)](#scene-transitions-changescene-pushscene-popscene)
  - [Layers](#layers)
    - [Layer Lifecycle](#layer-lifecycle)
    - [Overlays vs. Layers](#overlays-vs-layers)
    - [Layer Communication via Shared State](#layer-communication-via-shared-state)
  - [Events](#events)
    - [Event Propagation Through LayerStack](#event-propagation-through-layerstack)
    - [EventDispatcher: Type-Safe Handling](#eventdispatcher-type-safe-handling)
    - [EventBus: Pub/Sub Broadcasting](#eventbus-pubsub-broadcasting)
    - [Custom Events](#custom-events)
  - [Input](#input)
    - [Keyboard Polling](#keyboard-polling)
    - [Mouse Position and Buttons](#mouse-position-and-buttons)
    - [Gamepad Input](#gamepad-input)
    - [Text Input with KeyTypedEvent](#text-input-with-keytypedevent)
  - [Graphics](#graphics)
    - [Shapes with OriginMode](#shapes-with-originmode)
    - [Drawing Grids and Backgrounds](#drawing-grids-and-backgrounds)
    - [Colors and Interpolation](#colors-and-interpolation)
    - [Texts and FontManager](#texts-and-fontmanager)
  - [Math](#math)
    - [Vector2 Operations](#vector2-operations)
    - [Strongly-Typed Angles](#strongly-typed-angles)
    - [Math Utilities (Lerp, Clamp, Remap)](#math-utilities-lerp-clamp-remap)
    - [Random Numbers and Noise](#random-numbers-and-noise)
    - [Fractal Brownian Motion (Terrain Generation)](#fractal-brownian-motion-terrain-generation)
  - [ImGui Integration](#imgui-integration)
    - [Setting Up ImGuiLayer](#setting-up-imguilayer)
    - [Debug Panels with OnUIRender](#debug-panels-with-onuirender)
    - [Styling ImGui](#styling-imgui)
  - [Debug and Profiling](#debug-and-profiling)
    - [Instrumentation with RS_PROFILE_FUNCTION](#instrumentation-with-rs_profile_function)
    - [Scoped Profiling](#scoped-profiling)

---

## Quick Reference: Minimal Scene

A Raysim application has three mandatory moving parts: an `Application` constructed with an `ApplicationConfig`, at least one `Scene` registered with `RegisterScene<T>()`, and an active initial scene set with `ChangeScene<T>()`. The `RS::CreateApplication` free function is the framework's entry point — it is called by the engine's `main()` in `EntryPoint.hpp`.

`RS_SCENE(MyScene)` is a macro that injects the `StaticID()`, `StaticName()`, and a compatible constructor into your class. Without it, `RegisterScene<T>()` will not compile. See the [Scene System](#scene-system) section for details.

```cpp
#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MinimalScene : public Scene {
    RS_SCENE(MinimalScene)
private:
    void OnUpdate(float dt) override { /* game logic */ }
    void OnDraw(float /*alpha*/) override {
        GetRenderer().ClearScreen(Colors::DarkBlue);
        Shapes::DrawCircle(400.0f, 300.0f, 50.0f, Colors::RayWhite, OriginMode::Center);
    }
};

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args) {
    RS::ApplicationConfig config;
    config.Window.Title = "Minimal Demo";
    config.Window.Width = 800;
    config.Window.Height = 600;

    auto* app = new RS::Application(config);
    app->RegisterScene<MinimalScene>();
    app->ChangeScene<MinimalScene>();
    return app;
}
```

---

## Core

### Application Configuration

`ApplicationConfig` is a plain data struct filled before the `Application` constructor runs. Fields set here affect the engine's initial state; some can be changed at runtime through `GetWindow()` or `Time::*` calls, others (like backend selection) cannot.

```cpp
void OnStart() override {
    GetWindow().SetTitle("My Raysim App");
    GetWindow().SetSize(1280, 720);
    GetWindow().SetVSync(true);
    Time::SetTargetFPS(60);
    Time::SetFixedDeltaTime(1.0f / 120.0f); // 120 Hz physics
    Time::SetTimeScale(1.5f); // slow-mo / fast-forward
}
```

`ApplicationConfig` is passed to the `Application` constructor before `RegisterScene` is called. Settings applied here take effect before any scene is attached, making them suitable for window initialization:

```cpp
RS::ApplicationConfig config;
config.Window.Title  = "Configured App";
config.Window.Width = 1920;
config.Window.Height = 1080;
config.Log.Level    = RS::LogLevel::Debug;
config.MaxFixedStepsPerFrame = 16;
```

`MaxFixedStepsPerFrame` is a safety cap: if the application falls behind (e.g. a frame takes 500 ms), the fixed timestep accumulator is drained by at most this many steps before the frame is rendered. This prevents the "spiral of death" where a slow frame causes more fixed steps, which makes the next frame even slower.

### EngineContext Access

`EngineContext` is a non-owning aggregate of pointers to every core subsystem. Scenes receive it via their constructor and expose it through convenience methods. The underlying pointers are stable for the application's entire lifetime — they are never reallocated.

```cpp
class MyScene : public Scene {
    RS_SCENE(MyScene)
private:
    void OnStart() override {
        // Direct access via protected Scene members — preferred
        auto& window = GetWindow();
        auto& input  = GetInput();
        auto& scenes = GetSceneManager();
        auto& renderer = GetRenderer();
    }
};
```

Layers do not inherit from `Scene` and therefore do not have `GetWindow()` directly. The standard pattern is to pass required subsystem references through the layer constructor at the call site in `OnAttach`:

```cpp
class InputDisplayLayer : public Layer {
public:
    InputDisplayLayer(Input& input) : m_Input(input), Layer("InputDisplay") {}
    void OnUIRender() override {
        ImGui::Text("Mouse: (%d, %d)",
            (int)m_Input.GetMousePosition().x,
            (int)m_Input.GetMousePosition().y);
    }
private:
    Input& m_Input;
};
```

Passing by reference (not pointer) is intentional: the lifetime of `Input` is guaranteed to outlive any layer, so a reference is safe and removes one level of indirection from every method call.

### Time Utilities

The `Time` subsystem is a static facade backed by the engine's main loop timing. `dt` (delta time) passed to `OnUpdate` is already scaled by `TimeScale` — use it for game-speed-sensitive movement. `GetRealtimeSinceStartup()` is never scaled and is appropriate for UI animations, profiling, and cooldown timers that should not be affected by time scale or pausing:

```cpp
void OnUpdate(float dt) override {
    float fps   = Time::GetSmoothedFPS();      // exponential moving average, not instantaneous
    float gameT = Time::GetTime();             // scaled + paused-aware elapsed time
    float realT = Time::GetRealtimeSinceStartup(); // wall-clock, unaffected by scale or pause
    uint64_t frames = Time::GetFrameCount();

    // Time scaling: values > 1.0 speed up, < 1.0 slow down, 0.0 freezes game time
    Time::SetTimeScale(2.0f);

    // Pause/resume via toggle
    if (GetInput().IsKeyPressed(KeyCode::P))
        Time::IsPaused() ? Time::Resume() : Time::Pause();
}
```

### Logging

Raysim uses spdlog internally, exposed through two loggers: `CORE` (engine internals) and `CLIENT` (application code). The `RS_*` macros route to the client logger; `RS_CORE_*` macros route to the core logger. Both support `{}`-style fmt formatting. Log output goes to both the console and a file sink simultaneously.

All macros compile to zero-cost no-ops in Release builds when `RS_ENABLE_ASSERTS` and the relevant level flag are not defined:

```cpp
RS_TRACE("Trace message {}", 42);
RS_DEBUG("Debug message");
RS_INFO("Info message: {:.2f}", 3.14159f);
RS_WARN("Warning: memory usage high");
RS_ERROR("Error: failed to load font");
RS_CRITICAL("Critical: renderer lost device");

// Core logger (engine internals)
RS_CORE_INFO("Scene '{}' started", GetName());
RS_CORE_ERROR("Cannot change scene: not registered");
```

For high-frequency code paths, use the log-once and TTL-rate-limited variants to avoid flooding the log file. See `RS_LOG_ONCE` and `RS_LOG_TTL` macro documentation in `Assert.hpp`.

### Font Loading and Rendering

`FontManager` is a static registry. Fonts are identified by string handles (`"ui"`, `"title"`, etc.) assigned at load time. Loading is synchronous and must be done before any rendering that uses the font — typically in `OnStart`:

```cpp
void OnStart() override {
    // Load a custom TTF — path is relative to the working directory at runtime
    FontManager::LoadFont("ui", "fonts/OpenSans-Regular.ttf", 32);

    // Load the backend's default built-in font (no file required)
    FontManager::LoadDefaultFont();

    // Pre-measure text for layout calculations (avoids re-measuring every frame)
    Math::Vec2f size = FontManager::MeasureText(
        FontManager::GetFontHandle("ui"), "Hello World", 0.0f);
}
```

The integer `32` passed to `LoadFont` is the rasterization size in pixels. Raysim uses stb_truetype to rasterize the font into a GPU atlas at exactly this size — there is no runtime font scaling. Rendering at a size other than the rasterized size will produce blurry or incorrect output.

---

## Scene System

### Defining a Scene with RS_SCENE

Every scene class must include the `RS_SCENE(ClassName)` macro as its first member. The macro generates three things that `SceneManager` requires for registration and instantiation:

```cpp
class GameScene : public Scene {
    RS_SCENE(GameScene)
    // expands to:
    //   static SceneID StaticID();        — unique integer ID per type
    //   static const char* StaticName();  — "GameScene"
    //   GameScene(RS::EngineContext* ctx) : Scene(ctx) {}
};
```

The ID is generated using a compile-time type hash, so it is stable across builds as long as the class name does not change. `SceneManager` uses `StaticID()` for O(1) scene lookup by type. If you rename a scene class, any serialized scene IDs (e.g. in save files) will no longer match — keep this in mind for scenes that are persisted.

### Scene Lifecycle Walkthrough

Scenes have eight lifecycle callbacks. Understanding when each fires is essential for managing resources correctly:

```cpp
class LifecycleDemo : public Scene {
    RS_SCENE(LifecycleDemo)
    void OnAttach()   override { RS_INFO("Attached once"); }
    void OnStart()    override { RS_INFO("Started"); }
    void OnUpdate(float dt) override { /* per-frame */ }
    void OnFixedUpdate(float fixedDt) override { /* physics */ }
    void OnDraw(float alpha) override { /* render */ }
    void OnPause()    override { RS_INFO("Paused"); }
    void OnResume()   override { RS_INFO("Resumed"); }
    void OnDetach()   override { RS_INFO("Detached"); }
};
```

| Callback        | When it fires                                                   | Typical use                       |
| --------------- | --------------------------------------------------------------- | --------------------------------- |
| `OnAttach`      | Once, the first time the scene is pushed onto the stack         | One-time resource allocation      |
| `OnStart`       | After `OnAttach`, and again after every `OnResume`              | Reset state, reload data          |
| `OnUpdate`      | Every rendered frame; `dt` is scaled by `TimeScale`             | Game logic, animation             |
| `OnFixedUpdate` | At a fixed rate (default 120 Hz); `fixedDt` is constant         | Physics, deterministic simulation |
| `OnDraw`        | After all `OnUpdate` calls; `alpha` is the fixed-step remainder | Rendering only — no logic here    |
| `OnPause`       | When `SceneManager::PushScene` is called over this scene        | Mute audio, freeze AI             |
| `OnResume`      | When the scene on top is popped, exposing this scene again      | Restore audio, refresh UI         |
| `OnDetach`      | When the scene is removed from the stack                        | Free resources, cancel async work |

`OnFixedUpdate` runs zero or more times per rendered frame depending on elapsed time. The `alpha` parameter passed to `OnDraw` is the fractional remainder of the current fixed step — use it for interpolating physics positions to avoid jitter at display frame rates that differ from the fixed step rate.

### Scene Transitions (ChangeScene, PushScene, PopScene)

`SceneManager` manages a LIFO stack. `ChangeScene` replaces the current top-of-stack scene (calls `OnDetach` on the old scene, then `OnAttach`+`OnStart` on the new one). `PushScene` adds a new scene on top without removing the previous one — the previous scene receives `OnPause`. `PopScene` removes the top scene and resumes the one below it:

```cpp
class MainMenu : public Scene {
    RS_SCENE(MainMenu)
private:
    void OnUpdate(float dt) override {
        if (GetInput().IsKeyPressed(KeyCode::Enter)) {
            // Replace MainMenu entirely — OnDetach fires on MainMenu
            GetSceneManager().ChangeScene<GameScene>();
        }
        if (GetInput().IsKeyPressed(KeyCode::H)) {
            GetSceneManager().ChangeScene<HelpScene>();
        }
    }
};

class PauseScene : public Scene {
    RS_SCENE(PauseScene)
private:
    void OnAttach() override {
        PushOverlay(new PauseMenuLayer());
    }
    void OnUpdate(float dt) override {
        if (GetInput().IsKeyPressed(KeyCode::Escape)) {
            // Remove PauseScene — GameScene's OnResume fires
            GetSceneManager().PopScene();
        }
    }
};

// From GameScene: push PauseScene without stopping GameScene's OnUpdate
void OnUpdate(float dt) override {
    if (GetInput().IsKeyPressed(KeyCode::Escape)) {
        GetSceneManager().PushScene<PauseScene>();
    }
}
```

A common mistake is calling `ChangeScene` when `PushScene` is intended. If game state (score, position, NPC state) must be preserved while a menu is open, always use `PushScene` + `PopScene`. `ChangeScene` destroys the previous scene — there is no way to return to it.

---

## Layers

### Layer Lifecycle

Layers are sub-components of a scene that handle a single orthogonal concern (physics, audio, input translation, HUD rendering, etc.). They have the same `OnAttach` / `OnUpdate` / `OnDraw` / `OnDetach` callbacks as scenes, but are owned by the scene's `LayerStack` rather than the `SceneManager`. A layer's lifetime is scoped to the scene that created it — when the scene is detached, all its layers are detached and destroyed in reverse order.

```cpp
class PhysicsLayer : public Layer {
public:
    PhysicsLayer(float& pos, float& vel)
        : m_Pos(pos), m_Vel(vel), Layer("PhysicsLayer") {}

    void OnAttach() override { RS_DEBUG("Physics layer attached"); }
    void OnUpdate(float dt) override {
        m_Pos += m_Vel * dt;
    }
    void OnDetach() override { RS_DEBUG("Physics layer detached"); }
private:
    float& m_Pos;
    float& m_Vel;
};
```

Layer names (passed to the `Layer` constructor) appear in profiling output and debug overlays. Use a descriptive name — `"PhysicsLayer"` is better than `"Layer1"`.

### Overlays vs. Layers

The `LayerStack` processes layers in two distinct ordered groups:

- **Layers** (`PushLayer`): inserted before all overlays. Processed bottom-to-top during `OnUpdate`, `OnDraw`, and `OnEvent`. Use for gameplay systems that need to run before UI.
- **Overlays** (`PushOverlay`): always appended after all layers. Processed after layers in the same order. Use for UI, debug panels, or menus that must render on top.

Events flow from the **top** of the stack downward. An overlay always receives events before any layer. This matters for UI: a button overlay should consume click events before any game-world layer processes them.

```cpp
void OnAttach() override {
    // Game logic layer — processes input first among layers
    PushLayer(new GameLogicLayer());

    // HUD overlay — always on top, receives events first
    PushOverlay(new HudLayer());
    PushOverlay(new DebugLayer());
}
```

### Layer Communication via Shared State

Layers do not communicate directly with each other. The standard pattern is to pass references to shared state through each layer's constructor, letting the scene act as the coordinator:

```cpp
class GameScene : public Scene {
    RS_SCENE(GameScene)
    void OnStart() override {
        m_Score = 0;
        // Both layers reference the same m_Score variable
        PushLayer(new GameLogicLayer(m_Score));
        PushOverlay(new ScoreDisplayLayer(m_Score));
    }
private:
    int m_Score = 0;
};
```

For more complex interactions (e.g. an audio layer reacting to a collision event), use the `EventBus` instead of shared state — see [EventBus: Pub/Sub Broadcasting](#eventbus-pubsub-broadcasting).

---

## Events

### Event Propagation Through LayerStack

Raysim has two parallel event distribution mechanisms with different semantics, suited to different use cases:

**LayerStack propagation** (blocking, ordered): the engine walks the layer stack from top to bottom and calls `OnEvent(e)` on each layer. Any layer can stop propagation by setting `e.Handled = true`. This is appropriate for UI — a button that consumes a click prevents the world layer from also reacting to it.

**EventBus** (broadcast, unordered): all subscribers receive every matching event regardless of `e.Handled`. This is appropriate for decoupled systems (audio, achievements, analytics) that must react to game events without participating in the event-handling chain.

The propagation order for LayerStack events:

```txt
Window → [Overlay B] → [Overlay A] → [Layer C] → [Layer B] → [Layer A] → Scene → SceneManager
```

### EventDispatcher: Type-Safe Handling

`EventDispatcher` is a lightweight helper that pattern-matches an event against a concrete type. The lambda receives a typed reference and returns a `bool` indicating whether the event should be marked as handled. Returning `false` leaves the event open for downstream layers:

```cpp
class InputLayer : public Layer {
public:
    InputLayer(float& x, float& y) : m_X(x), m_Y(y), Layer("InputLayer") {}

    void OnEvent(Event& e) override {
        EventDispatcher d(e);
        d.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& ke) {
            if (ke.GetKeyCode() == KeyCode::W) m_Y -= 10.0f;
            if (ke.GetKeyCode() == KeyCode::S) m_Y += 10.0f;
            if (ke.GetKeyCode() == KeyCode::A) m_X -= 10.0f;
            if (ke.GetKeyCode() == KeyCode::D) m_X += 10.0f;
            return false; // let other layers also see this key event
        });
        d.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& me) {
            RS_TRACE("Mouse button pressed: {}", (int)me.GetButton());
            return true; // consume: no downstream layer should also handle this click
        });
    }
private:
    float& m_X;
    float& m_Y;
};
```

`EventDispatcher` uses `Event::GetEventType()` for the match, which is a single virtual call. There is no dynamic_cast involved.

### EventBus: Pub/Sub Broadcasting

`EventBus` subscriptions persist independently of the layer stack. A layer subscribing in `OnAttach` **must** unsubscribe in `OnDetach` — failing to do so will result in a dangling lambda being called after the layer is destroyed:

```cpp
class AudioLayer : public Layer {
public:
    AudioLayer() : Layer("AudioLayer") {}
    void OnAttach() override {
        // Subscribe returns an opaque ID needed for unsubscription
        m_Subscription = GetEventBus()->Subscribe<KeyPressedEvent>(
            [this](const KeyPressedEvent& e) {
                if (e.GetKeyCode() == KeyCode::Space)
                    PlayJumpSound();
            });
    }
    void OnDetach() override {
        // Required: the EventBus outlives this layer
        GetEventBus()->Unsubscribe(m_Subscription);
    }
private:
    EventBus::SubscriptionID m_Subscription;
};

class GameScene : public Scene {
    RS_SCENE(GameScene)
    void OnAttach() override {
        // Scenes can also emit events directly
        GetEventBus()->Emit<WindowResizeEvent>(
            GetWindow().GetWidth(), GetWindow().GetHeight());
    }
};
```

### Custom Events

Custom events inherit from `Event` and use two required macros to register their type with the dispatch system:

```cpp
// Events/GameEvents.hpp
#pragma once
#include "Raysim/Events/Event.hpp"

class ScoreChangedEvent : public Event {
public:
    explicit ScoreChangedEvent(int newScore, int previousScore)
        : m_NewScore(newScore), m_PreviousScore(previousScore) {}

    // Accessors use [[nodiscard]] to catch accidental unused-value bugs
    [[nodiscard]] int GetNewScore() const noexcept { return m_NewScore; }
    [[nodiscard]] int GetPreviousScore() const noexcept { return m_PreviousScore; }

    // Required macros — these define GetEventType(), GetStaticType(), and GetName()
    RS_EVENT_CLASS_TYPE(ScoreChanged)
    RS_EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    int m_NewScore;
    int m_PreviousScore;
};
```

Once defined, the event can be emitted and subscribed to like any built-in event:

```cpp
// Emit from anywhere that has EventBus access
GetEventBus()->Emit<ScoreChangedEvent>(m_Score, previousScore);

// Subscribe in any layer or scene
GetEventBus()->Subscribe<ScoreChangedEvent>([](const ScoreChangedEvent& e) {
    RS_INFO("Score: {} -> {}", e.GetPreviousScore(), e.GetNewScore());
});
```

---

## Input

### Keyboard Polling

The `Input` interface distinguishes four key states per frame:

- `IsKeyDown` — key is currently held (true every frame while held)
- `IsKeyPressed` — key transitioned from up to down this frame (true for one frame only)
- `IsKeyReleased` — key transitioned from down to up this frame (true for one frame only)
- `IsKeyRepeating` — key is held long enough to trigger OS key-repeat (useful for text editors, not game movement)

Use `IsKeyDown` for movement and continuous actions, `IsKeyPressed` for one-shot actions like jumping or toggling menus:

```cpp
void OnUpdate(float dt) override {
    // Movement with held keys
    float speed = 200.0f;
    Math::Vec2f dir{0.0f, 0.0f};
    if (GetInput().IsKeyDown(KeyCode::W)) dir.y -= 1.0f;
    if (GetInput().IsKeyDown(KeyCode::S)) dir.y += 1.0f;
    if (GetInput().IsKeyDown(KeyCode::A)) dir.x -= 1.0f;
    if (GetInput().IsKeyDown(KeyCode::D)) dir.x += 1.0f;
    m_Position += dir.Normalized() * speed * dt;

    // Single-press actions — fires exactly once per key-down transition
    if (GetInput().IsKeyPressed(KeyCode::F))
        ToggleFullscreen();

    if (GetInput().IsKeyPressed(KeyCode::R))
        GetSceneManager().ChangeScene<RestartScene>();
}
```

### Mouse Position and Buttons

`GetMouseDelta()` returns the movement since the last frame. It is zero if the mouse did not move and is not affected by `SetCursorPosition` calls. `GetMouseWheelMove()` returns the accumulated scroll delta for the current frame (positive = scroll up):

```cpp
void OnUpdate(float dt) override {
    Math::Vec2f mouse = GetInput().GetMousePosition();  // screen-space pixels
    Math::Vec2f delta = GetInput().GetMouseDelta();     // pixels moved this frame
    float wheel      = GetInput().GetMouseWheelMove();  // scroll units this frame

    if (GetInput().IsMouseButtonDown(MouseButton::Left)) {
        m_Dragging = true;
        m_DragStart = mouse;
    }

    if (GetInput().IsMouseButtonPressed(MouseButton::Right))
        SpawnContextMenu(mouse);

    // Camera zoom via scroll wheel
    m_CameraZoom += wheel * 0.1f;
    m_CameraZoom = Math::Clamp(m_CameraZoom, 0.5f, 5.0f);
}
```

### Gamepad Input

Gamepad indices start at `0`. Always check `IsGamepadAvailable(index)` before reading any gamepad state — querying a disconnected gamepad returns zeroes silently but the availability check makes the intent explicit and prevents confusion during debugging:

```cpp
void OnUpdate(float dt) override {
    if (!GetInput().IsGamepadAvailable(0)) return;

    // Thumb-stick movement — values in [-1, 1] with no built-in deadzone
    float lx = GetInput().GetGamepadAxisValue(0, GamepadAxis::LeftX);
    float ly = GetInput().GetGamepadAxisValue(0, GamepadAxis::LeftY);
    m_CameraPos.x += lx * 300.0f * dt;
    m_CameraPos.y += ly * 300.0f * dt;

    // Trigger values in [0, 1]
    float rt = GetInput().GetGamepadAxisValue(0, GamepadAxis::RightTrigger);
    m_Zoom += rt * 0.5f * dt;

    // Button press (one-shot) vs button down (continuous)
    if (GetInput().IsGamepadButtonPressed(0, GamepadButton::A))
        Interact();

    if (GetInput().IsGamepadButtonPressed(0, GamepadButton::Start))
        GetSceneManager().ChangeScene<PauseScene>();

    if (GetInput().IsGamepadButtonDown(0, GamepadButton::DPadUp))
        m_MenuIndex = Math::MaxValue(m_MenuIndex - 1, 0);
}
```

> Deadzones are not applied by the engine. Apply your own deadzone logic (e.g. `if (std::abs(lx) < 0.1f) lx = 0.0f;`) before using axis values for movement.

### Text Input with KeyTypedEvent

`KeyTypedEvent` carries a Unicode codepoint for each character committed by the OS input method. It is the correct way to implement text fields — it handles key combinations, dead keys, IME composition, and modifier state automatically, unlike reading individual `KeyCode` values:

```cpp
class TextInputLayer : public Layer {
public:
    TextInputLayer(std::string& buffer) : m_Buffer(buffer), Layer("TextInput") {}
    void OnEvent(Event& e) override {
        EventDispatcher d(e);
        d.Dispatch<KeyTypedEvent>([this](KeyTypedEvent& ke) {
            uint32_t cp = ke.GetCodepoint();
            if (cp == '\b') { // backspace
                if (!m_Buffer.empty()) m_Buffer.pop_back();
            } else if (cp >= 32 && cp < 127) {
                // Only accept printable ASCII for this example
                m_Buffer.push_back(static_cast<char>(cp));
            }
            return true; // consume — no other layer should see this character
        });
    }
private:
    std::string& m_Buffer;
};
```

---

## Graphics

### Shapes with OriginMode

Every `Shapes::Draw*` call accepts an `OriginMode` parameter that specifies which geometric point of the shape the `(x, y)` coordinates refer to. This eliminates the need for manual offset calculations when anchoring shapes to screen corners, centering them on a position, or aligning them to text baselines.

```cpp
// Draw a rectangle centered at (200, 200)
Shapes::DrawRect(200.0f, 200.0f, 100.0f, 50.0f, Colors::Cyan, OriginMode::Center);

// Draw a status bar pinned to the bottom-left corner
int w = GetWindow().GetWidth();
int h = GetWindow().GetHeight();
Shapes::DrawRect(10.0f, h - 60.0f, 200.0f, 50.0f, Colors::DarkGray, OriginMode::BottomLeft);

// Draw a circle outline that follows the mouse cursor exactly
Math::Vec2f mouse = GetInput().GetMousePosition();
Shapes::DrawCircleOutline(mouse.x, mouse.y, 20.0f, Colors::Yellow, OriginMode::Center);
```

| OriginMode     | Meaning of (x, y)                   |
| -------------- | ----------------------------------- |
| `TopLeft`      | top-left corner (default for rects) |
| `TopRight`     | top-right corner                    |
| `BottomLeft`   | bottom-left corner                  |
| `BottomRight`  | bottom-right corner                 |
| `TopCenter`    | top edge, horizontally centered     |
| `BottomCenter` | bottom edge, horizontally centered  |
| `CenterLeft`   | left edge, vertically centered      |
| `CenterRight`  | right edge, vertically centered     |
| `Center`       | geometric center                    |

### Drawing Grids and Backgrounds

Background rendering belongs in `OnDraw`, not `OnUpdate`. `ClearScreen` is the first call in `OnDraw` — it wipes the framebuffer before any geometry is drawn. Skipping it will cause the previous frame to bleed through:

```cpp
void DrawBackground() {
    GetRenderer().ClearScreen(Colors::DarkBlue);

    // Uniform pixel grid
    Color gridColor{60, 60, 60};
    for (int x = 0; x < GetWidth(); x += 50)
        Shapes::DrawLine(static_cast<float>(x), 0.0f,
                         static_cast<float>(x), static_cast<float>(GetHeight()),
                         gridColor);
    for (int y = 0; y < GetHeight(); y += 50)
        Shapes::DrawLine(0.0f, static_cast<float>(y),
                         static_cast<float>(GetWidth()), static_cast<float>(y),
                         gridColor);
}

// Radial vignette effect — concentric circles with decreasing alpha
void DrawRadialBackground() {
    Math::Vec2f center{ GetWidth() * 0.5f, GetHeight() * 0.5f };
    float maxR = Math::MaxValue(GetWidth(), GetHeight()) * 0.95f;
    for (int i = 0; i < 30; ++i) {
        float f     = static_cast<float>(i) / 30.0f;
        float r     = maxR * (1.0f - f * 0.95f);
        auto  alpha = static_cast<unsigned char>(Math::Remap(f, 0.0f, 1.0f, 200.0f, 5.0f));
        Shapes::DrawCircle(center.x, center.y, r,
                          {18, 24, 38, alpha}, OriginMode::Center);
    }
}
```

### Colors and Interpolation

`Color` is a four-component RGBA struct. `Color::FromHSV` converts hue/saturation/value to RGB at runtime, which is useful for rainbow animations. `Color::Lerp` interpolates linearly between two colors component-wise and clamps the result to `[0, 255]`:

```cpp
void OnUpdate(float dt) override {
    // Advance hue continuously — Repeat wraps the value back to [0, 360)
    m_Hue = Math::Repeat(m_Hue + 30.0f * dt, 360.0f);
    m_Color = Color::FromHSV(m_Hue, 0.8f, 1.0f);

    // PingPong produces a smooth 0→1→0 oscillation: useful for pulsing effects
    Color a{255, 0, 0};
    Color b{0, 0, 255};
    m_Blend = Color::Lerp(a, b, Math::PingPong(Time::GetTime(), 1.0f));
}

void OnDraw(float /*alpha*/) override {
    float pulse = Math::PingPong(Time::GetTime() * 3.0f, 1.0f);
    Shapes::DrawCircle(m_Pos.x, m_Pos.y, 20.0f + pulse * 10.0f, m_Color, OriginMode::Center);
}
```

### Texts and FontManager

`Text::RenderText` is a stateless call — it does not cache draw commands and does not batch automatically. For text that changes every frame (e.g. score counters, debug labels), this is fine. For text that changes infrequently, consider computing it in `OnUpdate` and storing it in a `std::string` member to avoid string formatting in `OnDraw`:

```cpp
void OnStart() override {
    FontManager::LoadFont("title", "fonts/Roboto-Bold.ttf", 64);
    FontManager::LoadFont("body",  "fonts/OpenSans-Regular.ttf", 24);
    FontManager::LoadFont("mono",  "fonts/FiraCode-Regular.ttf", 20);
}

void OnDraw(float /*alpha*/) override {
    // Center-align headline at the top of the screen
    Text::RenderText("Title", static_cast<float>(GetWidth()) * 0.5f, 50.0f,
                     64, Colors::White, OriginMode::TopCenter);

    // Bottom-left HUD label
    Text::RenderText("Controls: WASD to move, SPACE to jump",
                     20.0f, static_cast<float>(GetHeight()) - 40.0f,
                     24, Colors::LightGray, OriginMode::TopLeft);

    // Dynamic value — pre-format in OnUpdate if possible to avoid allocs here
    Text::RenderText(std::to_string(m_Score), m_ScorePos.x, m_ScorePos.y,
                     32, m_ScoreColor, OriginMode::Center);
}
```

---

## Math

### Vector2 Operations

`Math::Vec2f` is `Vector2<float>` and `Math::Vec2i` is `Vector2<int>`. All geometric methods return new vectors — they do not mutate in place. Methods like `Normalized()` return a zero-vector if the input length is zero (safe division), rather than producing NaN:

```cpp
void OnUpdate(float dt) override {
    Math::Vec2f pos{400.0f, 300.0f};
    Math::Vec2f vel{100.0f, -50.0f};

    // Basic arithmetic — operator overloads for +, -, *, /
    Math::Vec2f displacement = vel * dt;
    Math::Vec2f newPos = pos + displacement;

    // Normalization — returns zero-vector if length is zero
    Math::Vec2f dir = vel.Normalized();

    // Scalar products
    float dot   = dir.Dot(Math::Vec2f::UnitX());   // projection magnitude
    float cross = dir.Cross(Math::Vec2f::UnitY());  // signed area (2D "cross product")

    // Length and distance
    float len   = vel.Length();
    float dist  = pos.Distance(Math::Vec2f{200.0f, 100.0f});

    // Projection and rejection onto an axis
    Math::Vec2f axis{1.0f, 0.0f};
    Math::Vec2f proj  = vel.Project(axis);   // component along axis
    Math::Vec2f rej   = vel.Rejection(axis); // component perpendicular to axis

    // Angle utilities — all return strongly-typed Angle values
    Math::Angle angle        = vel.ToAngle();
    Math::Angle angleToTarget = pos.AngleTo(m_TargetPos);
    Math::Vec2f rotated       = vel.RotatedBy(45.0_deg);

    // Construct from angle
    Math::Vec2f fromAng = Math::Vec2f::FromAngle(90.0_deg); // ~{0, 1}
}
```

### Strongly-Typed Angles

`Angle` is a zero-overhead wrapper that stores its value internally in radians. All construction goes through factory methods, making the intended unit explicit at the call site. Implicit conversion from raw floats is intentionally disabled to prevent unit-mixing bugs:

```cpp
using namespace RS::Math;

// Construction via factory methods (explicit unit)
Angle a = Angle::Degrees(90.0f);
Angle b = Angle::Radians(Math::PI * 0.5f);

// User-defined literals (requires `using namespace RS::Math::Literals`)
Angle c = 45.0_deg;  // equivalent to Angle::Degrees(45.0f)

// Extraction
float deg = a.ToDegrees(); // ~90.0
float rad = b.ToRadians(); // ~1.5707

// Use in Vec2f rotation
Vec2f original{1.0f, 0.0f};
Vec2f rotated = original.RotatedBy(Angle::Degrees(90.0f)); // ~{0, 1}

// Angle between two vectors
Angle between = Vec2f::AngleBetween(original, rotated); // ~90°

// Arithmetic and wrapping
Angle sum     = a + b;               // ~180°
Angle diff    = a - b;               // ~0°
Angle scaled  = a * 2.0f;            // ~180°
Angle wrapped = (a * 5).WrapSigned(); // wraps to (-180°, 180°]
```

### Math Utilities (Lerp, Clamp, Remap)

`MathUtils.hpp` provides a collection of scalar utilities modeled after Unity's `Mathf` and Godot's `lerp`/`remap` family. All functions work with `float` unless otherwise noted:

```cpp
void OnUpdate(float dt) override {
    float t = Time::GetTime();

    // SmoothDamp — exponential moving average; smooth_time ~= time to reach target
    // m_Velocity is an in/out parameter maintained across frames
    m_Smoothed = Math::SmoothDamp(m_Smoothed, m_Target, m_Velocity, 0.1f, dt);

    // Linear interpolation and its inverse
    float blended = Math::Lerp(0.0f, 100.0f, 0.5f);          // 50.0
    float t_found = Math::InverseLerp(0.0f, 100.0f, 50.0f);  // 0.5

    // Remap: re-range a value from [a,b] to [c,d] linearly
    float remapped = Math::Remap(m_RawInput, -1.0f, 1.0f, 0.0f, 100.0f);

    // Clamping
    float clamped   = Math::Clamp(m_Value, 0.0f, 1.0f);
    float clamped01 = Math::Clamp01(m_Value);  // shorthand for Clamp(v, 0, 1)

    // Oscillation helpers — both return values in [0, length)
    float pingPong = Math::PingPong(t * 2.0f, 1.0f);  // 0→1→0→1→...
    float repeated = Math::Repeat(t, 1.0f);             // 0→1, 0→1, ...
    float fract    = Math::Fract(t);                    // fractional part of t

    // Smoothstep — cubic Hermite interpolation (zero derivative at endpoints)
    float smooth = Math::SmoothStep(0.0f, 1.0f, m_T);

    // MoveTowards — advances value by at most `step` per call, never overshoots
    float eased = Math::MoveTowards(m_From, m_To, 2.0f * dt);

    // Approximate equality using a configurable epsilon
    if (Math::EpsilonEquals(a, b, 1e-5f)) { /* treat as equal */ }

    float sat = Math::Saturate(m_Value); // equivalent to Clamp01
}
```

### Random Numbers and Noise

`Math::Random` is a static facade over a seeded `std::mt19937` generator. The seed is randomized via `std::random_device` on startup by default. Use `Seed(n)` for reproducible runs (e.g. in tests or procedural generation where replayability matters):

```cpp
void OnStart() override {
    Math::Random::Seed(12345); // reproducible sequence
}

void OnUpdate(float dt) override {
    if (GetInput().IsKeyPressed(KeyCode::R))
        Math::Random::SeedRandom(); // re-seed from hardware entropy
}

// Uniform distributions
int   i  = Math::Random::Int(100);            // [0, 100)
int   i2 = Math::Random::Range(-10, 10);      // [-10, 10)
float f  = Math::Random::Float();             // [0.0, 1.0)
float f2 = Math::Random::Range(0.0f, 10.0f); // [0.0, 10.0)
bool  b  = Math::Random::Bool();             // 50/50
bool  b2 = Math::Random::Bool(0.75f);        // 75% true probability

// Uniform container sampling (O(1), no copy)
std::vector<Color> palette = {Colors::Red, Colors::Green, Colors::Blue};
Color chosen = Math::Random::Choice(palette);

// Coherent noise functions (all values include the seed in the hash)
float perlin   = Math::Random::PerlinNoise(x * 0.1f, y * 0.1f);           // [-1, 1]
float simplex  = Math::Random::SimplexNoise(x * 0.1f, y * 0.1f);          // [-1, 1]
float cellular = Math::Random::CellularNoise(x * 0.1f, y * 0.1f);         // [0, 1]
float value    = Math::Random::ValueNoise(x * 0.1f, y * 0.1f);            // [0, 1]

// Fractal Brownian Motion — sums multiple octaves of noise for natural-looking terrain
float fbm = Math::Random::FractalBrownianMotion(
    x * 0.05f, y * 0.05f,
    6,     // octaves: more = finer detail, higher cost
    0.5f,  // persistence: amplitude scale per octave
    2.0f   // lacunarity: frequency scale per octave
);
```

### Fractal Brownian Motion (Terrain Generation)

FBM is the standard technique for generating natural-looking terrain, clouds, and other organic patterns. Each octave adds progressively finer detail at a lower amplitude. The result is remapped from `[-1, 1]` to `[0, 1]` before use as a normalized height value:

```cpp
void DrawTerrain(float scrollX, float skyHeight) {
    int w = GetWindow().GetWidth();
    int h = GetWindow().GetHeight();

    for (int x = 0; x < w; ++x) {
        float nx = (static_cast<float>(x) + scrollX) * 0.008f;
        float ny = static_cast<float>(x) * 0.008f;

        // FBM elevation in [-1, 1], remapped to [0, 1]
        float elevation = Math::Random::FractalBrownianMotion(nx, ny, 6, 0.5f, 2.0f);
        elevation = Math::Remap(elevation, -1.0f, 1.0f, 0.0f, 1.0f);

        int terrainY = static_cast<int>(skyHeight + elevation * (h - skyHeight));

        // Draw terrain column — color transitions from grass to soil with depth
        for (int y = terrainY; y < h; ++y) {
            float depth = static_cast<float>(y - terrainY) / (h - skyHeight);
            Color col = Color::Lerp(Color{34, 139, 34}, Color{139, 90, 43}, depth);
            Shapes::DrawPoint(static_cast<float>(x), static_cast<float>(y), 1.0f, col);
        }
    }
}
```

---

## ImGui Integration

### Setting Up ImGuiLayer

`SetupImGuiLayer()` is a convenience method defined on `Scene`. It calls `BackendFactory::CreateImGuiBackend()` internally, which selects the correct Dear ImGui backend for the active rendering backend at compile time. The returned pointer is an overlay that captures `OnUIRender` callbacks. Attach it during `OnStart`/`OnAttach`, before pushing any layer that will render UI:

```cpp
class ImGuiDemo : public Scene {
    RS_SCENE(ImGuiDemo)
private:
    ImGuiLayer* m_ImGui = nullptr;

    void OnStart() override {
        m_ImGui = SetupImGuiLayer();
        m_ImGui->ApplyStyle(ImGuiStyles::BuiltinStyle::Dark);

        // Layers pushed after the overlay appear below it in draw order
        PushOverlay(new DebugOverlayLayer());
        PushLayer(new ControlPanelLayer());
    }
};
```

### Debug Panels with OnUIRender

All Dear ImGui calls must be placed inside `OnUIRender`, not `OnDraw` or `OnUpdate`. The `ImGuiLayer` begins and ends the ImGui frame around this callback — calling ImGui functions outside it will produce no output or cause assertion failures inside the ImGui library:

```cpp
class DebugOverlayLayer : public Layer {
public:
    DebugOverlayLayer(float& fps, Math::Vec2f& mouse)
        : m_FPS(fps), m_Mouse(mouse), Layer("DebugOverlay") {}

    void OnUIRender() override {
        ImGui::Begin("Debug Panel");
        ImGui::Text("FPS: %.1f", m_FPS);
        ImGui::Text("Mouse: (%.0f, %.0f)", m_Mouse.x, m_Mouse.y);
        ImGui::Separator();

        static bool showGrid = true;
        if (ImGui::Checkbox("Show Grid", &showGrid))
            RS_TRACE("Grid toggled: {}", showGrid);

        ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.1f, 5.0f);
        if (ImGui::Button("Reset Camera"))
            ResetCamera();

        ImGui::End();
    }
private:
    float& m_FPS;
    Math::Vec2f& m_Mouse;
    float m_TimeScale = 1.0f;
};
```

### Styling ImGui

Raysim bundles several pre-registered visual themes. Call `ApplyStyle` with either a string name or a `BuiltinStyle` enum value. Switching themes at runtime is supported — useful for settings panels that let the user pick a look:

```cpp
void OnStart() override {
    m_ImGui = SetupImGuiLayer();

    // By style name (string)
    m_ImGui->ApplyStyle("Dark");

    // By built-in enum
    m_ImGui->ApplyStyle(ImGuiStyles::BuiltinStyle::Light);
}
```

Registered styles: `Dark`, `Light`, `Cherry`, `Dracula`, `Nord`, `Monokai`.

---

## Debug and Profiling

### Instrumentation with RS_PROFILE_FUNCTION

The profiler is opt-in at compile time. Define `RS_ENABLE_PROFILE` (e.g. via CMake `-DRS_ENABLE_PROFILE=ON`) to activate it; in release builds without the define, all profiling macros expand to no-ops with zero overhead.

When enabled, the profiler records the name, thread ID, start time, and duration of every instrumented scope, then writes a JSON results file (`RaysimProfile-*.json`) on shutdown. The format is compatible with Chrome DevTools (open `chrome://tracing`) and Perfetto — both can visualize the call timeline as a flame chart.

Place `RS_PROFILE_FUNCTION()` at the top of any function whose cost you want to measure. It inserts a scoped timer whose name is the compiler-supplied function signature:

```cpp
void OnUpdate(float dt) override {
    RS_PROFILE_FUNCTION();
    // ... expensive simulation update ...
}
```

### Scoped Profiling

When you need finer granularity than whole functions, use `RS_PROFILE_SCOPE("label")` to time individual blocks. Scopes nest correctly — the profiler records each independently, so you can see both the total cost of `OnDraw` and the cost of each sub-operation within it:

```cpp
void OnDraw(float alpha) override {
    RS_PROFILE_FUNCTION(); // measure OnDraw as a whole

    {
        RS_PROFILE_SCOPE("DrawTerrain"); // measure this block separately
        DrawTerrain();
    }

    {
        RS_PROFILE_SCOPE("DrawParticles");
        DrawParticles();
    }

    RS_PROFILE_SCOPE("DrawUI"); // inline scope — ends at function return
    DrawHUD();
}
```

Each scope records its own timing independently of the call stack, giving flat cost profiles for every instrumented function.

---

<p align="center">
  <img src="images/footer.png" alt="Raysim Footer" width="600" />
</p>
