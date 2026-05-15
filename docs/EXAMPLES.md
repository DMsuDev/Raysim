# Examples

Practical, in-depth examples for every module in the Raysim framework. Each section explains the underlying mechanism first, then shows the corresponding code.

---

## Table of Contents

- [Quick Reference](#quick-reference-minimal-scene) - Minimal scene example
- [Core](#core) - Application, Time, Logging, Fonts
- [Scene System](#scene-system) - Scene lifecycle and transitions
- [Layers](#layers) - Layer management and communication
- [Events](#events) - Event propagation and custom events
- [Input](#input) - Keyboard, mouse, gamepad
- [Graphics](#graphics) - Shapes, colors, text
- [Math](#math) - Vectors, angles, noise
- [ImGui](#imgui-integration) - Debug panels and styling
- [Debug](#debug-and-profiling) - Profiling and instrumentation

## Quick Reference: Minimal Scene

A Raysim application has three mandatory parts:

1. An `Application` constructed with `ApplicationConfig`
2. At least one `Scene` registered with `RegisterScene<T>()`
3. An active initial scene set with `ChangeScene<T>()`

### Minimal Working Example

```cpp
#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MinimalScene : public Scene {
    RS_SCENE(MinimalScene)  // Required macro - DO NOT forget!
private:
    void OnUpdate(float dt) override { /* game logic */ }
    void OnDraw(float /*alpha*/) override {
        renderer().ClearScreen(Colors::DarkBlue);
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

## Core

### Application Configuration

`ApplicationConfig` is a plain data struct filled **before** the `Application` constructor runs. Some settings can be changed at runtime, others (like backend selection) cannot.

```cpp
RS::ApplicationConfig config;
config.Window.Title  = "Configured App";
config.Window.Width  = 1920;
config.Window.Height = 1080;
config.Log.Level     = RS::LogLevel::Debug;
config.TargetFPS     = 144;
config.Vsync         = true;
```

### Runtime Configuration

```cpp
void OnStart() override {
    window().SetTitle("My Raysim App");
    window().SetSize(1280, 720);
    window().SetVSync(true);
    Time::SetTargetFPS(60);
    Time::SetFixedDeltaTime(1.0f / 120.0f); // 120 Hz physics
    Time::SetTimeScale(1.5f); // slow-mo / fast-forward
}
```

> MaxFixedStepsPerFrame is a safety cap: if the application falls behind (e.g. a frame takes 500 ms), the fixed timestep accumulator is drained by at most this many steps before the frame is rendered.

### EngineContext Access

Scenes receive `EngineContext` via their constructor and expose it through convenience methods (e.g., `window()`, `input()`, `sceneManager()`, `renderer()`).

```cpp
class MyScene : public Scene {
    RS_SCENE(MyScene)
private:
    void OnStart() override {
        auto& window = window();
        auto& input  = input();
        auto& scenes = sceneManager();
        auto& renderer = renderer();
    }
};
```

#### Accessing from Layers

Layers do **not** inherit from `Scene`, so they don't have `window()` directly. Pass required subsystem references through the layer constructor:

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

---

### Time Utilities

`dt` (delta time) passed to `OnUpdate` is already scaled by `TimeScale`. `GetRealtimeSinceStartup()` is never scaled — use it for UI animations and profiling.

```cpp
void OnUpdate(float dt) override {
    float fps   = Time::GetSmoothedFPS();      // exponential moving average
    float gameT = Time::GetTime();             // scaled + paused-aware
    float realT = Time::GetRealtimeSinceStartup(); // wall-clock time
    uint64_t frames = Time::GetFrameCount();

    // Time scaling
    Time::SetTimeScale(2.0f);

    // Pause/resume
    if (input().IsKeyPressed(KeyCode::P))
        Time::IsPaused() ? Time::Resume() : Time::Pause();
}
```

---

### Logging

Raysim uses spdlog internally with two loggers: `CORE` (engine internals) and `CLIENT` (application code).

| Macro                           | Logger | Purpose               |
| ------------------------------- | ------ | --------------------- |
| `RS_TRACE`, `RS_DEBUG`, etc.    | CLIENT | Your application code |
| `RS_CORE_INFO`, `RS_CORE_ERROR` | CORE   | Engine internals      |

```cpp
// Client logger (your code)
RS_TRACE("Trace message {}", 42);
RS_DEBUG("Debug message");
RS_INFO("Info: {:.2f}", 3.14159f);
RS_WARN("Warning: memory usage high");
RS_ERROR("Error: failed to load font");
RS_CRITICAL("Critical: renderer lost device");

// Core logger (engine internals)
RS_CORE_INFO("Scene '{}' started", GetName());
RS_CORE_ERROR("Cannot change scene: not registered");
```

All macros compile to zero-cost no-ops in Release builds when assertions are disabled.

---

### Font Loading and Rendering

Fonts are identified by string handles (`"ui"`, `"title"`, etc.). Loading is synchronous and must be done before any rendering — typically in `OnStart`.

```cpp
void OnStart() override {
    // Load a custom TTF
    FontManager::LoadFont("ui", "fonts/OpenSans-Regular.ttf", 32);

    // Load the backend's default built-in font
    FontManager::LoadDefaultFont();

    // Pre-measure text for layout calculations
    Math::Vec2f size = FontManager::MeasureText(
        FontManager::GetFontHandle("ui"), "Hello World", 0.0f);
}
```

> The integer passed to `LoadFont` is the rasterization size in pixels. Raysim uses stb_truetype to rasterize at exactly this size — there is no runtime font scaling.

---

## Scene System

### Defining a Scene with RS_SCENE

Every scene class **must** include `RS_SCENE(ClassName)` as its first member. This macro generates three things required for registration.

```cpp
class GameScene : public Scene {
    RS_SCENE(GameScene)
    // Expands to:
    //   static SceneID StaticID();        — unique integer ID
    //   static const char* StaticName();  — "GameScene"
    //   GameScene(RS::EngineContext* ctx) : Scene(ctx) {}
};
```

The ID is generated using a compile-time type hash, so it is stable across builds as long as the class name does not change.

---

### Scene Lifecycle Walkthrough

Scenes have eight lifecycle callbacks. Understanding when each fires is essential for managing resources correctly.

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

| Callback        | When it fires                                | Typical use                       |
| --------------- | -------------------------------------------- | --------------------------------- |
| `OnAttach`      | Once, first time pushed onto stack           | One-time resource allocation      |
| `OnStart`       | After `OnAttach`, and after every `OnResume` | Reset state, reload data          |
| `OnUpdate`      | Every rendered frame                         | Game logic, animation             |
| `OnFixedUpdate` | At fixed rate (default 120 Hz)               | Physics, deterministic simulation |
| `OnDraw`        | After all `OnUpdate` calls                   | Rendering only                    |
| `OnPause`       | When `PushScene` is called over this scene   | Mute audio, freeze AI             |
| `OnResume`      | When scene on top is popped                  | Restore audio, refresh UI         |
| `OnDetach`      | When removed from stack                      | Free resources, cancel async work |

The `alpha` parameter in `OnDraw` is the fractional remainder of the current fixed step — use it for interpolating physics positions to avoid jitter.

---

### Scene Transitions

> Common mistake: calling `ChangeScene` when `PushScene` is intended. Use `PushScene` + `PopScene` when game state must be preserved.

```cpp
class MainMenu : public Scene {
    RS_SCENE(MainMenu)
private:
    void OnUpdate(float dt) override {
        if (input().IsKeyPressed(KeyCode::Enter)) {
            sceneManager().ChangeScene<GameScene>();  // Replace entirely
        }
        if (input().IsKeyPressed(KeyCode::H)) {
            sceneManager().ChangeScene<HelpScene>();
        }
    }
};

class PauseScene : public Scene {
    RS_SCENE(PauseScene)
private:
    void OnUpdate(float dt) override {
        if (input().IsKeyPressed(KeyCode::Escape)) {
            sceneManager().PopScene();  // Remove PauseScene
        }
    }
};

// From GameScene: push PauseScene without stopping GameScene's OnUpdate
void OnUpdate(float dt) override {
    if (input().IsKeyPressed(KeyCode::Escape)) {
        sceneManager().PushScene<PauseScene>();
    }
}
```

`ChangeScene` replaces the current scene (calls `OnDetach` on old, `OnAttach`+`OnStart` on new). `PushScene` adds on top without removing previous (`OnPause` on previous). `PopScene` removes top and resumes the one below.

---

## Layers

### Layer Lifecycle

Layers are sub-components of a scene that handle a single orthogonal concern. They have the same callbacks as scenes but are owned by the scene's `LayerStack`.

```cpp
class PhysicsLayer : public Layer {
public:
    PhysicsLayer(float& pos, float& vel)
        : m_Pos(pos), m_Vel(vel), Layer("PhysicsLayer") {}

    void OnAttach() override { RS_DEBUG("Physics layer attached"); }
    void OnUpdate(float dt) override { m_Pos += m_Vel * dt; }
    void OnDetach() override { RS_DEBUG("Physics layer detached"); }
private:
    float& m_Pos;
    float& m_Vel;
};
```

Layer names appear in profiling output and debug overlays. Use descriptive names — `"PhysicsLayer"` is better than `"Layer1"`.

---

### Overlays vs. Layers

The `LayerStack` processes layers in two distinct ordered groups:

| Type     | Method        | Order               | Use Case                                    |
| -------- | ------------- | ------------------- | ------------------------------------------- |
| Layers   | `PushLayer`   | Before all overlays | Gameplay systems that need to run before UI |
| Overlays | `PushOverlay` | After all layers    | UI, debug panels, menus                     |

Events flow from the **top** of the stack downward. An overlay always receives events before any layer.

```cpp
void OnAttach() override {
    PushLayer(new GameLogicLayer());     // Processed first among layers
    PushOverlay(new HudLayer());         // Always on top
    PushOverlay(new DebugLayer());
}
```

---

### Layer Communication via Shared State

Layers do **not** communicate directly. The standard pattern is to pass references to shared state through each layer's constructor:

```cpp
class GameScene : public Scene {
    RS_SCENE(GameScene)
    void OnStart() override {
        m_Score = 0;
        PushLayer(new GameLogicLayer(m_Score));
        PushOverlay(new ScoreDisplayLayer(m_Score));
    }
private:
    int m_Score = 0;
};
```

For complex interactions (e.g., audio reacting to collision), use the **EventBus** instead.

---

## Events

### Event Propagation Through LayerStack

Raysim has two parallel event distribution mechanisms:

1. **LayerStack propagation** (blocking, ordered) — any layer can stop propagation by setting `e.Handled = true`. Suitable for UI.
2. **EventBus** (broadcast, unordered) — all subscribers receive every matching event. Suitable for decoupled systems (audio, achievements).

> Propagation order: Window → Overlay B → Overlay A → Layer C → Layer B → Layer A → Scene

---

### EventDispatcher: Type-Safe Handling

`EventDispatcher` is a lightweight helper that pattern-matches an event against a concrete type.

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
            return false; // let other layers also see this
        });
        d.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& me) {
            RS_TRACE("Mouse: {}", (int)me.GetButton());
            return true; // consume: no downstream layer should handle
        });
    }
private:
    float& m_X;
    float& m_Y;
};
```

---

### EventBus: Pub/Sub Broadcasting

> A layer subscribing in `OnAttach` **must** unsubscribe in `OnDetach` — failing to do so results in a dangling lambda being called after the layer is destroyed.

```cpp
class AudioLayer : public Layer {
public:
    AudioLayer() : Layer("AudioLayer") {}
    void OnAttach() override {
        m_Subscription = GetEventBus()->Subscribe<KeyPressedEvent>(
            [this](const KeyPressedEvent& e) {
                if (e.GetKeyCode() == KeyCode::Space)
                    PlayJumpSound();
            });
    }
    void OnDetach() override {
        GetEventBus()->Unsubscribe(m_Subscription);
    }
private:
    EventBus::SubscriptionID m_Subscription;
};
```

---

### Custom Events

```cpp
// Events/GameEvents.hpp
#pragma once
#include "Raysim/Events/Event.hpp"

class ScoreChangedEvent : public Event {
public:
    explicit ScoreChangedEvent(int newScore, int previousScore)
        : m_NewScore(newScore), m_PreviousScore(previousScore) {}

    [[nodiscard]] int GetNewScore() const noexcept { return m_NewScore; }
    [[nodiscard]] int GetPreviousScore() const noexcept { return m_PreviousScore; }

    RS_EVENT_CLASS_TYPE(ScoreChanged)
    RS_EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    int m_NewScore;
    int m_PreviousScore;
};
```

Use `[[nodiscard]]` to catch accidental unused-value bugs.

---

## Input

### Keyboard Polling

The `Input` interface distinguishes four key states per frame:

| Method           | Behavior                                              |
| ---------------- | ----------------------------------------------------- |
| `IsKeyDown`      | Key is currently held (true every frame)              |
| `IsKeyPressed`   | Key transitioned from up to down (true for one frame) |
| `IsKeyReleased`  | Key transitioned from down to up (true for one frame) |
| `IsKeyRepeating` | OS key-repeat triggered                               |

```cpp
void OnUpdate(float dt) override {
    // Movement with held keys
    float speed = 200.0f;
    Math::Vec2f dir{0.0f, 0.0f};
    if (input().IsKeyDown(KeyCode::W)) dir.y -= 1.0f;
    if (input().IsKeyDown(KeyCode::S)) dir.y += 1.0f;
    if (input().IsKeyDown(KeyCode::A)) dir.x -= 1.0f;
    if (input().IsKeyDown(KeyCode::D)) dir.x += 1.0f;
    m_Position += dir.Normalized() * speed * dt;

    // Single-press actions
    if (input().IsKeyPressed(KeyCode::F)) ToggleFullscreen();
    if (input().IsKeyPressed(KeyCode::R)) sceneManager().ChangeScene<RestartScene>();
}
```

---

### Mouse Position and Buttons

```cpp
void OnUpdate(float dt) override {
    Math::Vec2f mouse = input().GetMousePosition();  // screen-space pixels
    Math::Vec2f delta = input().GetMouseDelta();     // moved this frame
    float wheel      = input().GetMouseWheelMove();  // scroll units

    if (input().IsMouseButtonDown(MouseButton::Left)) {
        m_Dragging = true;
        m_DragStart = mouse;
    }

    if (input().IsMouseButtonPressed(MouseButton::Right))
        SpawnContextMenu(mouse);

    m_CameraZoom += wheel * 0.1f;
    m_CameraZoom = Math::Clamp(m_CameraZoom, 0.5f, 5.0f);
}
```

---

### Gamepad Input

Gamepad indices start at `0`. Always check `IsGamepadAvailable(index)` before reading any gamepad state.

```cpp
void OnUpdate(float dt) override {
    if (!input().IsGamepadAvailable(0)) return;

    // Thumb-stick movement — values in [-1, 1] with no built-in deadzone
    float lx = input().GetGamepadAxisValue(0, GamepadAxis::LeftX);
    float ly = input().GetGamepadAxisValue(0, GamepadAxis::LeftY);
    m_CameraPos.x += lx * 300.0f * dt;
    m_CameraPos.y += ly * 300.0f * dt;

    // Trigger values in [0, 1]
    float rt = input().GetGamepadAxisValue(0, GamepadAxis::RightTrigger);

    if (input().IsGamepadButtonPressed(0, GamepadButton::A)) Interact();
    if (input().IsGamepadButtonPressed(0, GamepadButton::Start))
        sceneManager().ChangeScene<PauseScene>();
}
```

> Deadzones are not applied by the engine. Apply your own: `if (std::abs(lx) < 0.1f) lx = 0.0f;`

---

### Text Input with KeyTypedEvent

`KeyTypedEvent` carries Unicode codepoints. It handles key combinations, dead keys, IME composition automatically — use this for text fields.

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
                m_Buffer.push_back(static_cast<char>(cp));
            }
            return true; // consume
        });
    }
private:
    std::string& m_Buffer;
};
```

---

## Graphics

### Shapes with OriginMode

Every `Shapes::Draw*` call accepts an `OriginMode` parameter that specifies which geometric point of the shape the `(x, y)` coordinates refer to.

```cpp
// Centered rectangle
Shapes::DrawRect(200.0f, 200.0f, 100.0f, 50.0f, Colors::Cyan, OriginMode::Center);

// Pinned to bottom-left
int w = window().GetWidth();
int h = window().GetHeight();
Shapes::DrawRect(10.0f, h - 60.0f, 200.0f, 50.0f, Colors::DarkGray, OriginMode::BottomLeft);

// Circle following mouse
Math::Vec2f mouse = input().GetMousePosition();
Shapes::DrawCircleOutline(mouse.x, mouse.y, 20.0f, Colors::Yellow, OriginMode::Center);
```

| OriginMode     | Meaning                            |
| -------------- | ---------------------------------- |
| `TopLeft`      | top-left corner (default)          |
| `TopRight`     | top-right corner                   |
| `BottomLeft`   | bottom-left corner                 |
| `BottomRight`  | bottom-right corner                |
| `TopCenter`    | top edge, horizontally centered    |
| `BottomCenter` | bottom edge, horizontally centered |
| `CenterLeft`   | left edge, vertically centered     |
| `CenterRight`  | right edge, vertically centered    |
| `Center`       | geometric center                   |

---

### Drawing Grids and Backgrounds

Background rendering belongs in `OnDraw`, not `OnUpdate`. Call `ClearScreen` first — skipping it causes the previous frame to bleed through.

```cpp
void DrawBackground() {
    renderer().ClearScreen(Colors::DarkBlue);

    Color gridColor{60, 60, 60};
    for (int x = 0; x < GetWidth(); x += 50)
        Shapes::DrawLine(x, 0.0f, x, GetHeight(), gridColor);
    for (int y = 0; y < GetHeight(); y += 50)
        Shapes::DrawLine(0.0f, y, GetWidth(), y, gridColor);
}

void DrawRadialBackground() {
    Math::Vec2f center{ GetWidth() * 0.5f, GetHeight() * 0.5f };
    float maxR = Math::MaxValue(GetWidth(), GetHeight()) * 0.95f;
    for (int i = 0; i < 30; ++i) {
        float f = static_cast<float>(i) / 30.0f;
        float r = maxR * (1.0f - f * 0.95f);
        auto alpha = static_cast<unsigned char>(Math::Remap(f, 0.0f, 1.0f, 200.0f, 5.0f));
        Shapes::DrawCircle(center.x, center.y, r, {18, 24, 38, alpha}, OriginMode::Center);
    }
}
```

---

### Colors and Interpolation

```cpp
void OnUpdate(float dt) override {
    // Rainbow animation — wraps hue to [0, 360)
    m_Hue = Math::Repeat(m_Hue + 30.0f * dt, 360.0f);
    m_Color = Color::FromHSV(m_Hue, 0.8f, 1.0f);

    // Oscillating blend
    Color a{255, 0, 0}, b{0, 0, 255};
    m_Blend = Color::Lerp(a, b, Math::PingPong(Time::GetTime(), 1.0f));
}
```

---

### Texts and FontManager

```cpp
void OnStart() override {
    FontManager::LoadFont("title", "fonts/Roboto-Bold.ttf", 64);
    FontManager::LoadFont("body",  "fonts/OpenSans-Regular.ttf", 24);
    FontManager::LoadFont("mono",  "fonts/FiraCode-Regular.ttf", 20);
}

void OnDraw(float /*alpha*/) override {
    // Center-align headline
    Text::RenderText("Title", GetWidth() * 0.5f, 50.0f, 64, Colors::White, OriginMode::TopCenter);

    // Bottom-left HUD
    Text::RenderText("Controls: WASD + SPACE", 20.0f, GetHeight() - 40.0f, 24, Colors::LightGray, OriginMode::TopLeft);

    // Dynamic value — pre-format in OnUpdate
    Text::RenderText(std::to_string(m_Score), m_ScorePos.x, m_ScorePos.y, 32, m_ScoreColor, OriginMode::Center);
}
```

`Text::RenderText` is stateless. For text that changes infrequently, compute it in `OnUpdate` and store in a string member.

---

## Math

### Vector2 Operations

`Math::Vec2f` is `Vector2<float>`. All geometric methods return new vectors — they do **not** mutate in place.

```cpp
void OnUpdate(float dt) override {
    Math::Vec2f pos{400.0f, 300.0f};
    Math::Vec2f vel{100.0f, -50.0f};

    // Basic arithmetic
    Math::Vec2f displacement = vel * dt;
    Math::Vec2f newPos = pos + displacement;

    // Normalization — returns zero-vector if length is zero (safe)
    Math::Vec2f dir = vel.Normalized();

    // Scalar products
    float dot   = dir.Dot(Math::Vec2f::UnitX());
    float cross = dir.Cross(Math::Vec2f::UnitY());

    // Length and distance
    float len   = vel.Length();
    float dist  = pos.Distance({200.0f, 100.0f});

    // Projection and rejection
    Math::Vec2f axis{1.0f, 0.0f};
    Math::Vec2f proj  = vel.Project(axis);
    Math::Vec2f rej   = vel.Rejection(axis);

    // Angle utilities
    Math::Angle angle = vel.ToAngle();
    Math::Angle angleToTarget = pos.AngleTo(m_TargetPos);
    Math::Vec2f rotated = vel.RotatedBy(45.0_deg);
    Math::Vec2f fromAng = Math::Vec2f::FromAngle(90.0_deg);
}
```

---

### Strongly-Typed Angles

`Angle` is a zero-overhead wrapper that stores its value internally in radians. Implicit conversion from raw floats is intentionally **disabled** to prevent unit-mixing bugs.

```cpp
using namespace RS::Math;

// Construction via factory methods
Angle a = Angle::Degrees(90.0f);
Angle b = Angle::Radians(Math::PI * 0.5f);

// User-defined literals (requires `using namespace RS::Math::Literals`)
Angle c = 45.0_deg;

// Extraction
float deg = a.ToDegrees(); // ~90.0
float rad = b.ToRadians(); // ~1.5707

// Use in Vec2f
Vec2f rotated = Vec2f::UnitX().RotatedBy(Angle::Degrees(90.0f));

// Arithmetic
Angle wrapped = (a * 5).WrapSigned(); // wraps to (-180°, 180°]
```

---

### Math Utilities

```cpp
void OnUpdate(float dt) override {
    // SmoothDamp — exponential moving average
    m_Smoothed = Math::SmoothDamp(m_Smoothed, m_Target, m_Velocity, 0.1f, dt);

    // Interpolation
    float blended = Math::Lerp(0.0f, 100.0f, 0.5f);       // 50.0
    float t_found = Math::InverseLerp(0.0f, 100.0f, 50.0f); // 0.5
    float remapped = Math::Remap(m_RawInput, -1.0f, 1.0f, 0.0f, 100.0f);

    // Clamping
    float clamped = Math::Clamp(m_Value, 0.0f, 1.0f);
    float clamped01 = Math::Clamp01(m_Value);

    // Oscillation
    float pingPong = Math::PingPong(Time::GetTime() * 2.0f, 1.0f);  // 0→1→0→1→...
    float repeated = Math::Repeat(Time::GetTime(), 1.0f);          // 0→1, 0→1, ...
    float fract = Math::Fract(Time::GetTime());

    // MoveTowards — never overshoots
    float eased = Math::MoveTowards(m_From, m_To, 2.0f * dt);
}
```

---

### Random Numbers and Noise

```cpp
void OnStart() override {
    Math::Random::Seed(12345); // reproducible sequence
}

void OnUpdate(float dt) override {
    if (input().IsKeyPressed(KeyCode::R))
        Math::Random::SeedRandom(); // re-seed from hardware entropy
}

// Uniform distributions
int   i  = Math::Random::Int(100);            // [0, 100)
int   i2 = Math::Random::Range(-10, 10);      // [-10, 10)
float f  = Math::Random::Float();             // [0.0, 1.0)
bool  b  = Math::Random::Bool(0.75f);        // 75% true

// Container sampling (O(1), no copy)
std::vector<Color> palette = {Colors::Red, Colors::Green, Colors::Blue};
Color chosen = Math::Random::Choice(palette);

// Coherent noise functions
float perlin   = Math::Random::PerlinNoise(x * 0.1f, y * 0.1f);   // [-1, 1]
float simplex  = Math::Random::SimplexNoise(x * 0.1f, y * 0.1f);  // [-1, 1]
float cellular = Math::Random::CellularNoise(x * 0.1f, y * 0.1f); // [0, 1]

// Fractal Brownian Motion
float fbm = Math::Random::FractalBrownianMotion(
    x * 0.05f, y * 0.05f,
    6,     // octaves
    0.5f,  // persistence
    2.0f   // lacunarity
);
```

---

### Fractal Brownian Motion (Terrain Generation)

FBM is the standard technique for generating natural-looking terrain, clouds, and organic patterns. Each octave adds finer detail at lower amplitude.

```cpp
void DrawTerrain(float scrollX, float skyHeight) {
    int w = window().GetWidth();
    int h = window().GetHeight();

    for (int x = 0; x < w; ++x) {
        float nx = (static_cast<float>(x) + scrollX) * 0.008f;
        float ny = static_cast<float>(x) * 0.008f;

        float elevation = Math::Random::FractalBrownianMotion(nx, ny, 6, 0.5f, 2.0f);
        elevation = Math::Remap(elevation, -1.0f, 1.0f, 0.0f, 1.0f);

        int terrainY = static_cast<int>(skyHeight + elevation * (h - skyHeight));

        for (int y = terrainY; y < h; ++y) {
            float depth = static_cast<float>(y - terrainY) / (h - skyHeight);
            Color col = Color::Lerp({34, 139, 34}, {139, 90, 43}, depth);
            Shapes::DrawPoint(x, y, 1.0f, col);
        }
    }
}
```

---

## ImGui Integration

### Setting Up ImGuiLayer

Attach ImGuiLayer during `OnStart`/`OnAttach`, before pushing any layer that will render UI.

```cpp
class ImGuiDemo : public Scene {
    RS_SCENE(ImGuiDemo)
private:
    ImGuiLayer* m_ImGui = nullptr;

    void OnStart() override {
        m_ImGui = SetupImGuiLayer();
        m_ImGui->ApplyStyle(ImGuiStyles::BuiltinStyle::Dark);
        PushOverlay(new DebugOverlayLayer());
        PushLayer(new ControlPanelLayer());
    }
};
```

---

### Debug Panels with OnUIRender

All Dear ImGui calls must be placed inside `OnUIRender`, not `OnDraw` or `OnUpdate`. Calling ImGui outside produces no output or assertion failures.

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
            RS_TRACE("Grid: {}", showGrid);

        ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.1f, 5.0f);
        if (ImGui::Button("Reset Camera")) ResetCamera();

        ImGui::End();
    }
private:
    float& m_FPS;
    Math::Vec2f& m_Mouse;
    float m_TimeScale = 1.0f;
};
```

---

### Styling ImGui

```cpp
void OnStart() override {
    m_ImGui = SetupImGuiLayer();

    // By style name
    m_ImGui->ApplyStyle("Dark");

    // By enum
    m_ImGui->ApplyStyle(ImGuiStyles::BuiltinStyle::Light);
}
```

Registered styles: `Dark`, `Light`, `Cherry`, `Dracula`, `Nord`, `Monokai`

---

## Debug and Profiling

### Instrumentation with RS_PROFILE_FUNCTION

The profiler is opt-in at compile time. Define `RS_ENABLE_PROFILE` (e.g., via CMake `-DRS_ENABLE_PROFILE=ON`). In release builds without the define, all profiling macros expand to no-ops.

```cpp
void OnUpdate(float dt) override {
    RS_PROFILE_FUNCTION();
    // ... expensive simulation update ...
}
```

---

### Scoped Profiling

Use `RS_PROFILE_SCOPE("label")` to time individual blocks. Scopes nest correctly — the profiler records each independently.

```cpp
void OnDraw(float alpha) override {
    RS_PROFILE_FUNCTION(); // measure OnDraw as a whole

    {
        RS_PROFILE_SCOPE("DrawTerrain");
        DrawTerrain();
    }

    {
        RS_PROFILE_SCOPE("DrawParticles");
        DrawParticles();
    }

    RS_PROFILE_SCOPE("DrawUI");
    DrawHUD();
}
```

Results are written to `RaysimProfile-*.json` on shutdown. The format is compatible with **Chrome DevTools** (`chrome://tracing`) and **Perfetto**.

---

<p align="center">
  <img src="images/footer.png" alt="Raysim Footer" width="720" />
</p>
