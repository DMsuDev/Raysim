# Backends

This document explains how _Raysim’s_ backend architecture works and how to implement a new backend.  
It covers the purpose of the abstraction, the responsibilities of each interface, and the exact steps required to integrate a new graphics or input platform into the engine.

> **Navigation:** [Home](../README.md) • [Examples](./EXAMPLES.md) • Backends

## Table of Contents

- [Backends](#backends)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [What Is a Backend?](#what-is-a-backend)
    - [Why Five Separate Interfaces?](#why-five-separate-interfaces)
  - [Architecture](#architecture)
    - [Backend Selection at Configure Time](#backend-selection-at-configure-time)
    - [BackendFactory: Compile-Time Dispatch](#backendfactory-compile-time-dispatch)
    - [Interface Hierarchy](#interface-hierarchy)
    - [Directory Layout](#directory-layout)
  - [Interfaces](#interfaces)
  - [RendererAPI](#rendererapi)
  - [Window](#window)
  - [Input and EventDrivenInput](#input-and-eventdriveninput)
    - [Polling Model (Raylib-style)](#polling-model-raylib-style)
    - [Event-Driven Model (GLFW, SDL2, Win32)](#event-driven-model-glfw-sdl2-win32)
  - [ImGuiBackend](#imguibackend)
  - [FontRenderer](#fontrenderer)
    - [KeyMap Template](#keymap-template)
    - [KeyCode Mappings](#keycode-mappings)
    - [MouseButton Mappings](#mousebutton-mappings)
    - [Gamepad Mappings](#gamepad-mappings)
  - [Step-by-Step: Creating a New Backend](#step-by-step-creating-a-new-backend)
    - [1. Create the Backend Directory](#1-create-the-backend-directory)
    - [2. Implement the KeyMap Header](#2-implement-the-keymap-header)
    - [3. Implement RendererAPI](#3-implement-rendererapi)
    - [4. Implement Window](#4-implement-window)
    - [5. Implement Input / EventDrivenInput](#5-implement-input--eventdriveninput)
      - [Option A — Polling Backend](#option-a--polling-backend)
      - [Option B — Event-Driven Backend](#option-b--event-driven-backend)
    - [6. Implement ImGuiBackend](#6-implement-imguibackend)
    - [7. Implement FontRenderer](#7-implement-fontrenderer)
    - [8. Wire Up BackendFactory](#8-wire-up-backendfactory)
    - [9. Update CMake Configuration](#9-update-cmake-configuration)
  - [Platform-Specific Considerations](#platform-specific-considerations)
    - [Win32](#win32)
    - [macOS](#macos)
    - [Linux](#linux)

---

## Overview

### What Is a Backend?

`Raysim` is fully backend‑agnostic at the user level. Every draw call, window operation, and input query goes through abstract C++ interfaces — the engine core never interacts with a graphics or windowing API directly.
A backend is simply the concrete implementation of those interfaces.

The `Raylib` backend is the reference implementation and the only officially supported one _for now_. It handles window creation, input polling, frame begin/end, font atlas uploads, and ImGui integration by delegating to `raylib` and `rlImGui`.
The architecture is intentionally simple so that a developer can implement additional backends (e.g., `SDL3` + `OpenGL`, `GLFW` + `Vulkan`, or a headless software renderer) by following this document.

### Why Five Separate Interfaces?

> [!NOTE]
> Backend responsibilities are intentionally split across **five narrow interfaces**, rather than a single monolithic "platform" class.

Each interface has its own lifetime, caller, and thread‑safety expectations:

| Interface      | Abstract Base  | Lifetime             | Primary Caller         |
| -------------- | -------------- | -------------------- | ---------------------- |
| `RendererAPI`  | `RendererAPI`  | Application lifetime | Main thread, per-frame |
| `Window`       | `Window`       | Application lifetime | Main thread            |
| `Input`        | `Input`        | Application lifetime | Scene / Layer          |
| `ImGuiBackend` | `ImGuiBackend` | Application lifetime | ImGui Layer            |
| `FontRenderer` | `FontRenderer` | Application lifetime | FontManager            |

This separation allows you to replace or stub individual components independently.

For example, you can keep Raylib’s `Window` + `Input` while replacing only `FontRenderer` with a custom GPU implementation, without touching other backends.

## Architecture

### Backend Selection at Configure Time

The backend is selected at CMake configure time through the `RS_BACKEND` cache variable.  
This choice is fixed for the lifetime of the build directory, `Raysim` does not support switching backends at runtime.

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DRS_BACKEND=raylib     # raylib (default), sdl2, glfw, etc.
```

CMake reads `RS_BACKEND`, defines the corresponding preprocessor macro (e.g. `RS_BACKEND_RAYLIB`), and compiles only the source files and third‑party libraries for that backend.

All other backend directories are excluded entirely, no unused code ends up in the final binary.

### BackendFactory: Compile-Time Dispatch

`BackendFactory` is the single point of construction for all backend objects.

It uses `#if defined(...)` blocks that resolve entirely at compile time, no virtual dispatch, no `std::variant`, no type-erasure beyond the abstract interface itself:

```cpp
// BackendFactory.cpp
Scope<RendererAPI> BackendFactory::CreateRenderer() {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibRendererAPI>();
    #elif defined(RS_BACKEND_SDL2)
        return CreateScope<RS::Backend::SDL2RendererAPI>();
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}
```

Each `Create*` function follows the same pattern. `BackendFactory` holds no state, it is used once during `Application` startup to populate the engine’s subsystem pointers.

The macro is defined in `CMakeLists.txt` after reading `RS_BACKEND`:

```cmake
option(RS_BACKEND "Backend to use" "raylib")

if(RS_BACKEND STREQUAL "raylib")
    add_compile_definitions(RS_BACKEND_RAYLIB)
    # ...
endif()
```

### Interface Hierarchy

Understanding ownership and call order is critical for implementing a backend correctly. The engine follows a strict ownership tree rooted at `Application`:

```txt
Application
  ├── owns: Window (abstract*)
  ├── owns: Input (abstract*)
  ├── owns: EventBus
  ├── owns: SceneManager
  └── owns: RenderCommand (static facade)
                └── owns: RendererAPI (abstract*)

Scene → LayerStack → Layer
  └── each Layer overrides: OnUpdate, OnDraw, OnEvent, OnUIRender
```

`*` = constructed by `BackendFactory`, stored as `Scope<T>` (i.e. `std::unique_ptr<T>`), injected into the engine at startup. **No backend headers are visible in user-facing headers.** The entire concrete backend type is hidden behind the abstract interface pointer — scenes and layers only ever see the abstract base.

This means the application code is completely decoupled from the backend. Swapping a backend requires zero changes to user code; only the CMake configure step changes.

### Directory Layout

The physical layout mirrors the logical separation: public interface headers live under `include/Raysim/`, concrete backend headers live under `include/Backend/<BackendName>/`, and implementation files live under `src/Backend/<BackendName>/`:

```txt
include/Backend/
    Raylib/
        RaylibRendererAPI.hpp
        RaylibWindow.hpp
        RaylibInput.hpp
        RaylibImGuiBackend.hpp
        RaylibFontRenderer.hpp
        RaylibKeyMap.hpp
    BackendKeyMapTemplate.hpp    ← starting point for new backends
```

Each backend subdirectory contains:

- `XxxRendererAPI.hpp/.cpp` — implements `RendererAPI`
- `XxxWindow.hpp/.cpp` — implements `Window`
- `XxxInput.hpp/.cpp` — implements `Input` or `EventDrivenInput`
- `XxxImGuiBackend.hpp/.cpp` — implements `ImGuiBackend`
- `XxxFontRenderer.hpp/.cpp` — implements `FontRenderer`
- `XxxKeyMap.hpp` — compile-time translation tables (header-only)

## Interfaces

> [!TIP]
> Before implementing a new backend, familiarize yourself with the five core interfaces below. Each has a specific responsibility and lifetime within the engine.

## RendererAPI

**Header**: `Raysim/Renderer/RendererAPI.hpp`

`RendererAPI` is the engine’s lowest-level graphics abstraction.  
It provides only the essential operations required by the core:

- Frame lifecycle (begin/end frame)
- Viewport control
- Screen clearing

All drawing operations (shapes, sprites, text) are implemented by higher-level
systems (`RenderCommand`, `Shapes`, `Texts`) using the backend’s native API
(e.g., raylib’s drawing functions).

> [!NOTE]
> The interface is intentionally minimal. It will expand as new backends or
> advanced features (render targets, GPU resources, sync primitives) are added.

---

**Method Summary**

| Method          | Signature                                                                      | Responsibility                                                                | Call Frequency           |
| --------------- | ------------------------------------------------------------------------------ | ----------------------------------------------------------------------------- | ------------------------ |
| `Init()`        | `virtual void Init() {}`                                                       | Optional backend setup (shaders, buffers, state). Called once after creation. | Once at startup          |
| `BeginFrame()`  | `virtual void BeginFrame() = 0`                                                | Start a new frame. Bind targets, reset state, prepare for rendering.          | Once per frame           |
| `EndFrame()`    | `virtual void EndFrame() = 0`                                                  | Finish rendering and present the frame (swap buffers / submit).               | Once per frame           |
| `SetViewport()` | `virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0` | Define the renderable region of the target.                                   | On demand                |
| `ClearScreen()` | `virtual void ClearScreen(const Color& color) = 0`                             | Clear the color buffer to a specific color.                                   | Typically once per frame |
| `Clear()`       | `virtual void Clear() = 0`                                                     | Clear depth/stencil (or backend equivalent).                                  | On demand                |

## Window

**Header**: `Raysim/Core/Window.hpp`

`Window` implements a **Template Method** architecture:
The public API consists of non-virtual methods that perform bookkeeping (e.g. updating `m_Props`) and then delegate to protected pure virtual `Impl*` functions implemented by each backend.

This ensures that `WindowProps` (title, width, height, VSync, fullscreen flags) always remains consistent, regardless of the underlying platform.

```cpp
class Window {
protected:
    Window(const WindowProps& props); // called by subclasses

public:
    void PollEvents();                  // pumps OS event queue
    void SwapBuffers();                 // presents rendered frame
    bool ShouldClose() const;

    void SetSize(int w, int h);
    Vec2i GetSize() const;

    void SetTitle(const std::string& title);
    void SetVSync(bool enabled);
    bool IsVSync() const;

    void SetFullscreen(bool fs);
    bool IsFullscreen() const;

    bool IsMinimized() const;
    void GetFramebufferSize(int& width, int& height) const;

    void* GetNativeWindow() const;       // platform handle (HWND, GLXWindow, etc.)
    const WindowProps& GetProps() const;
};
```

Backends implement the protected `Impl*` methods:

```cpp
protected:
    virtual void ImplPollEvents() = 0;
    virtual void ImplSwapBuffers() = 0;
    virtual bool ImplShouldClose() const = 0;

    virtual void ImplSetSize(int w, int h) = 0;
    virtual Vec2i ImplGetSize() const = 0;

    virtual void ImplSetTitle(const std::string& title) = 0;
    virtual void ImplSetFullscreen(bool fs) = 0;
    virtual bool ImplIsMinimized() const = 0;
    virtual void ImplSetVSync(bool enabled) = 0;

    virtual void* ImplGetNativeWindow() const = 0;
```

The base `Window` class synthesizes and dispatches:

- `WindowResizeEvent`
- `WindowCloseEvent`
- `WindowFocusEvent`

Backends **must not** re‑implement event dispatch.
Instead, they call the base class helpers (e.g., `FireWindowResizeEvent(w, h)`) whenever the platform reports a relevant event.
This centralizes event behavior and prevents subtle inconsistencies between backends.

> [!IMPORTANT]
> `GetNativeWindow()` returns a raw platform handle (`HWND`, `GLFWwindow*`, etc.) as a `void*` on purpose.
> This keeps backend types out of the public headers.
> Casting should only be done inside backend implementations, never in user code.

## Input and EventDrivenInput

**Header**: `Raysim/Input/Input.hpp`

> [!NOTE]
> Raysim distinguishes two fundamentally different input models, which map to two distinct base classes.

### Polling Model (Raylib-style)

The backend directly queries the native API on every call. No state is accumulated between queries. `IsKeyDown(key)` asks raylib _right now_ whether that key is held.

> [!TIP]
> This model is simpler to implement but only works if the native library provides per-frame polling functions.

### Event-Driven Model (GLFW, SDL2, Win32)

The native library delivers input through callbacks or an event queue. The backend must accumulate per-frame state (down/pressed/released/repeating) in arrays, update them as callbacks fire, and reset one-shot states (pressed/released) at the start of each frame.

> [!IMPORTANT]
> `EventDrivenInput` provides all of this machinery; backends only need to implement the native callback bridge.

The abstract `Input` interface covers both models:

```cpp
// For polling backends (Raylib-style)
class Input : public Object {
public:
    virtual bool IsKeyDown(KeyCode key) const = 0;
    virtual bool IsKeyPressed(KeyCode key) const = 0;
    virtual bool IsKeyReleased(KeyCode key) const = 0;
    virtual bool IsKeyRepeating(KeyCode key) const = 0;
    virtual KeyCode GetLastKeyPressed() const = 0;
    virtual uint32_t GetCharPressed() { return 0; }

    virtual Math::Vec2f GetMousePosition() const = 0;
    virtual Math::Vec2f GetMouseDelta() const = 0;
    virtual float GetMouseWheelMove() const = 0;
    virtual bool IsMouseButtonDown(MouseButton btn) const = 0;
    virtual bool IsMouseButtonPressed(MouseButton btn) const = 0;
    virtual bool IsMouseButtonReleased(MouseButton btn) const = 0;

    virtual void SetCursorPosition(float x, float y) = 0;
    virtual void SetCursorVisible(bool visible) = 0;
    virtual void SetCursorMode(CursorMode mode) = 0;
    virtual CursorMode GetCursorMode() const = 0;

    virtual bool IsGamepadAvailable(int gamepad) const = 0;
    virtual bool IsGamepadButtonDown(int gamepad, GamepadButton btn) const = 0;
    virtual bool IsGamepadButtonPressed(int gamepad, GamepadButton btn) const = 0;
    virtual bool IsGamepadButtonReleased(int gamepad, GamepadButton btn) const = 0;
    virtual float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const = 0;
    virtual float GetGamepadAxisValue(int gamepad, int axis) const = 0;
    virtual const char* GetGamepadName(int gamepad) const { return ""; }
};
```

**EventDrivenInput** (`Raysim/Input/EventDrivenInput.hpp`) provides the state arrays and frame-reset logic. Concrete backends that use this base class only need to call the protected setter methods from their native callbacks — they never manipulate the arrays directly:

```cpp
class EventDrivenInput : public Input {
protected:
    void SetKeyDown(KeyCode key, bool down);
    void SetMouseButtonDown(MouseButton btn, bool down);
    void SetMousePosition(float x, float y);
    void SetMouseWheel(float delta);
    void SetCharPressed(uint32_t codepoint);

    EventBus::SubscriptionID m_Subscription; // for window events

public:
    void OnEvent(Event& e); // feeds events into the state arrays
    void Update() override; // clears pressed/released states each frame
};
```

The `OnEvent` handler processes incoming events from the `EventBus` and maps them to state transitions:

- `KeyPressedEvent` → `SetKeyDown(key, true)`, sets the `IsRepeat` flag if `e.IsRepeat()` is true
- `KeyReleasedEvent` → `SetKeyDown(key, false)`, clears the repeat flag
- `MouseButtonPressedEvent` / `MouseButtonReleasedEvent` → toggle mouse button state
- `MouseMovedEvent` → updates position and accumulates delta for the current frame
- `MouseScrolledEvent` → accumulates wheel delta
- `KeyTypedEvent` → pushes a codepoint to the character queue (consumed by `GetCharPressed()`)

`Update()` is called once per frame before `OnUpdate` runs. It copies the _down_ state to compute _pressed_ (newly down this frame) and _released_ (newly up this frame), then clears those one-shot arrays for next frame.

## ImGuiBackend

**Header**: `Raysim/ImGui/ImGuiBackend.hpp`

> [!IMPORTANT]
> The ImGui backend is responsible for exactly **two** things:
>
> - **BeginFrame**: Forward platform state to `ImGuiIO` (cursor, keys, display size)
> - **EndFrame**: Issue GPU draw calls for the ImGui draw lists
>
> These map directly to Dear ImGui's two-phase rendering model.

```cpp
class ImGuiBackend {
public:
    virtual ~ImGuiBackend() = default;
    virtual void Init(Window& window) = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0; // platform-side: feed cursor, keys, display size to ImGuiIO
    virtual void EndFrame() = 0;  // renderer-side: render ImGui draw lists
};
```

`Init` receives a reference to the active `Window` so it can extract the native window handle for installing callbacks (on GLFW/SDL2) or querying the framebuffer size. The backend is responsible for creating the `ImGui` context, configuring `ImGuiIO::BackendPlatformName`, and initializing any GPU-side resources (shader programs, vertex buffers, sampler states).

A typical lifecycle for an event-driven ImGui backend (GLFW + OpenGL3) looks like:

```cpp
void MyImGuiBackend::Init(Window& window) {
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformUserData = this;
    io.BackendPlatformName = "imgui_impl_mybackend";

    // Install GLFW callbacks — ImGuiIO will accumulate input from these
    auto* native = static_cast<GLFWwindow*>(window.GetNativeWindow());
    glfwSetMouseButtonCallback(native, MyMouseButtonCallback);
    glfwSetScrollCallback(native, MyScrollCallback);
    glfwSetKeyCallback(native, MyKeyCallback);
    glfwSetCharCallback(native, MyCharCallback);

    // Init GPU renderer backend
    ImGui_ImplOpenGL3_Init("#version 130");
}

void MyImGuiBackend::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLFW_NewFrame();   // updates display size, delta time, cursor shape
    ImGui::NewFrame();
}

void MyImGuiBackend::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // If multi-viewport is enabled, additional platform windows need to be
    // rendered here as well — see Dear ImGui's multi-viewport documentation.
}
```

> Note: the Raysim Raylib backend uses `rlImGui` (bundled in `third_party/rlImGui/`) which wraps both `BeginFrame` and `EndFrame` responsibilities in `rlImGuiBegin()` / `rlImGuiEnd()`.

## FontRenderer

**Header**: `Raysim/Fonts/Rendering/FontRenderer.hpp`

> [!NOTE]
> `FontRenderer` is the **GPU-side half** of the font system. `FontManager` owns the atlas metadata (glyph rectangles, advance widths, kerning) and calls `FontRenderer` to upload atlas textures to GPU memory and issue text draw calls.

This split allows `FontManager` to be **backend-agnostic**: it never touches a GPU API directly.

```cpp
class FontRenderer {
public:
    virtual ~FontRenderer() = default;

    virtual void Init() = 0;
    virtual void Shutdown() = 0;

    virtual void LoadFontAtlas(const Font& font) = 0;
    virtual void UnloadFontAtlas(FontHandle handle) = 0;

    virtual void RenderText(FontHandle handle,
                            const std::string& text,
                            float x, float y,
                            float fontSize,
                            float spacing,
                            const Color& color) = 0;
};
```

A correct `LoadFontAtlas` implementation must:

1. Retrieve the pixel data from `font.Atlas` (an RGBA or alpha-only bitmap produced by stb_truetype).
2. Upload it to GPU memory as a 2D texture and store the GPU texture ID in `font.Atlas.TextureID`.
3. Keep the texture alive until `UnloadFontAtlas` is called — `FontManager` may hold `FontHandle` values that span multiple frames.

`RenderText` receives a `FontHandle` (an opaque index into `FontManager`'s internal table), looks up the corresponding `FontAtlas` and `Glyph` array, then computes per-character screen-space quads and issues GPU draw calls (OpenGL batched quads, raylib `DrawTextureRec`, etc.).

The `FontAtlas` struct (`Raysim/Fonts/Types/FontAtlas.hpp`) provides all the data needed for rendering:

- `std::vector<Glyph>` — one entry per codepoint: UV rectangle, bearing, advance width
- `unsigned int TextureID` — GPU texture handle (populated by `LoadFontAtlas`)
- `int AtlasWidth`, `int AtlasHeight` — atlas dimensions in pixels

### KeyMap Template

> [!IMPORTANT]
> Every backend must translate between Raysim's abstract key/button/axis codes and the native integers used by the underlying library.

This translation is done **entirely at compile time** through `constexpr` arrays — there are no `switch` statements, no hash maps, no runtime lookups.

> [!TIP]
> The `BackendKeyMapTemplate.hpp` in `include/Backend/` is a fully commented template. Copy it to your backend directory, rename it, and fill in the native constants. The `static_assert` checks at the bottom of each array definition will catch any mismatch between the array size and the enum's `Count` sentinel.

---

### KeyCode Mappings

Every Raysim `KeyCode` (from `Raysim/Input/KeyCodes.hpp`) must have a corresponding native integer at the same array index. Index 0 is always `KeyCode::Unknown` (maps to `0`). The array is sized exactly `KeyCode::Count`, enforced by `static_assert`:

```cpp
inline constexpr std::array<int, static_cast<std::size_t>(KeyCode::Count)> KeyCodeToXxx = {{
    0,    // 0    Unknown
    32,   // 1    Space              ← native key code for Space in your library
    39,   // 2    Apostrophe
    44,   // 3    Comma
    // ... one entry per KeyCode, in enum declaration order ...
}};
static_assert(KeyCodeToXxx.size() == static_cast<std::size_t>(KeyCode::Count),
    "KeyCodeToXxx size mismatch — update this array when KeyCode enum changes");
```

A reverse lookup table (`XxxKeyToKeyCode`) is built from this array at compile time to translate native scancodes back to Raysim `KeyCode` values during event processing:

```cpp
// Built automatically from KeyCodeToXxx by the template
inline constexpr auto XxxKeyToKeyCode = BuildReverseKeyMap(KeyCodeToXxx);
```

### MouseButton Mappings

> [!NOTE]
> Mouse button indices are small and fixed. Map any unsupported button to `-1`.

```cpp
inline constexpr std::array<int, MouseButtonCount> MouseButtonToXxx = {{
    0,  // 0    Left
    1,  // 1    Right
    2,  // 2    Middle
   -1,  // 3    (unsupported — map to -1)
   -1,  // 4    (unsupported)
   -1,  // 5    (unsupported)
   -1,  // 6    (unsupported)
   -1,  // 7    (unsupported)
}};
```

At runtime, any call to `IsMouseButtonDown(-1)` is treated as always-false by the `Input` base class — no bounds violation occurs.

### Gamepad Mappings

Gamepad buttons use an Xbox-style layout (`GamepadButton` enum). Axes use the standard `GamepadAxis` enum (`LeftX`, `LeftY`, `RightX`, `RightY`, `LeftTrigger`, `RightTrigger`). Unsupported entries map to `-1`:

```cpp
inline constexpr std::array<int, XxxGamepadButtonCount> GamepadButtonToXxx = {{
   -1,  // 0    Unknown
    0,  // 1    DPadUp
    3,  // 2    DPadRight
    1,  // 3    DPadDown
    2,  // 4    DPadLeft
    4,  // 5    Y / Triangle
    1,  // 6    B / Circle
    0,  // 7    A / Cross
    2,  // 8    X / Square
   // ...
}};

inline constexpr std::array<int, XxxGamepadAxisCount> GamepadAxisToXxx = {{
    0,  // 0    LeftX
    1,  // 1    LeftY
    2,  // 2    RightX
    3,  // 3    RightY
    4,  // 4    LeftTrigger
    5,  // 5    RightTrigger
}};
```

---

## Step-by-Step: Creating a New Backend

> [!IMPORTANT]
> The steps below are **ordered to minimize integration friction**. Start with the KeyMap (no compilation dependency on anything else), then work inward toward `BackendFactory`.

---

### 1. Create the Backend Directory

Create the following files. Implementation (`.cpp`) files go under `src/`, interface headers under `include/Backend/`:

```txt
include/Backend/MyBackend/
    MyBackendRendererAPI.hpp
    MyBackendWindow.hpp
    MyBackendInput.hpp
    MyBackendImGuiBackend.hpp
    MyBackendFontRenderer.hpp
    MyBackendKeyMap.hpp          ← copy and fill in BackendKeyMapTemplate.hpp
src/Backend/MyBackend/
    MyBackendRendererAPI.cpp
    MyBackendWindow.cpp
    MyBackendInput.cpp
    MyBackendImGuiBackend.cpp
    MyBackendFontRenderer.cpp
```

---

### 2. Implement the KeyMap Header

> [!TIP]
> Copy `include/Backend/BackendKeyMapTemplate.hpp` to `include/Backend/MyBackend/MyBackendKeyMap.hpp`. Fill in every entry in the three translation arrays (`KeyCodeToXxx`, `MouseButtonToXxx`, `GamepadButtonToXxx`, `GamepadAxisToXxx`) using the native constants from your window/input library.

The `static_assert` lines at the end of each array will fail at compile time if you miss any entry.

> [!CAUTION]
> The KeyMap header must be `#include`d only from within the backend `.cpp` files — it should **never** appear in engine-facing headers.

---

### 3. Implement RendererAPI

> [!NOTE]
> `RendererAPI` is the **simplest** interface.

Implement `BeginFrame` / `EndFrame` as the frame delimiters of your graphics API, `ClearScreen` as a full-framebuffer clear, and `SetViewport` as the equivalent of `glViewport`:

```cpp
// MyBackendRendererAPI.hpp
#pragma once
#include "Raysim/Renderer/RendererAPI.hpp"

namespace RS::Backend {
class MyBackendRendererAPI final : public RendererAPI {
public:
    void Init() override;
    void BeginFrame() override;
    void EndFrame() override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
    void ClearScreen(const Color& color) override;
    void ClearScreen(const Math::Vec3f& color) override;
    void Clear() override;
};
} // namespace RS::Backend
```

`Init()` is the right place to initialize any GPU state that must be set up once (load GLAD, create shared VAOs/UBOs, compile shaders used by `FontRenderer`, etc.).

---

### 4. Implement Window

> [!WARNING]
> Inherit from `Window` and override every `Impl*` virtual. Do **not** fire events from inside the `Impl*` methods directly — call the protected helper methods provided by the `Window` base class (e.g. `FireWindowResizeEvent`, `FireWindowCloseEvent`) so the base class can update `m_Props` before dispatching:

```cpp
// MyBackendWindow.hpp
#pragma once
#include "Raysim/Core/Window.hpp"

namespace RS::Backend {
class MyBackendWindow final : public Window {
public:
    MyBackendWindow(const WindowProps& props);
    ~MyBackendWindow() override;

protected:
    void ImplPollEvents() override;
    void ImplSwapBuffers() override;
    bool ImplShouldClose() const override;
    void ImplSetSize(int w, int h) override;
    Math::Vec2i ImplGetSize() const override;
    void ImplSetTitle(const std::string& title) override;
    void ImplSetFullscreen(bool fs) override;
    bool ImplIsMinimized() const override;
    void ImplSetVSync(bool enabled) override;
    void* ImplGetNativeWindow() const override;

private:
    void* m_NativeWindow = nullptr;
    void* m_NativeContext = nullptr;  // OpenGL context handle, if separate from window
};
} // namespace RS::Backend
```

The constructor must call the base `Window(props)` constructor first. Inside it, create the OS window, create the graphics context (or let the native library do it), and apply the initial `VSync` and `Fullscreen` settings from `props`.

---

### 5. Implement Input / EventDrivenInput

> [!IMPORTANT]
> Choose the input model that matches your library:

#### Option A — Polling Backend

_(library provides `IsKeyDown`-style queries per frame)_

```cpp
class MyBackendInput : public Input {
public:
    bool IsKeyDown(KeyCode key) const override {
        int native = MyBackendKeyMap::KeyCodeToNative[static_cast<int>(key)];
        return native != -1 && MyLib_IsKeyDown(native);
    }
    bool IsKeyPressed(KeyCode key) const override { /* same pattern */ }
    Math::Vec2f GetMousePosition() const override { /* query native */ }
    // ... implement all pure virtuals
};
```

#### Option B — Event-Driven Backend

_(library delivers input via callbacks)_

> [!TIP]
> Inherit from `EventDrivenInput` and install native callbacks that call the protected setters. The base class handles **all** state accumulation and frame-reset logic:

```cpp
class MyBackendInput : public EventDrivenInput {
public:
    MyBackendInput(GLFWwindow* window);
    ~MyBackendInput() override;

private:
    // Called from GLFW key callback
    void OnNativeKeyEvent(int nativeKey, int action, int mods) {
        KeyCode key = MyBackendKeyMap::NativeToKeyCode(nativeKey);
        bool repeat = (action == GLFW_REPEAT);
        bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        SetKeyDown(key, pressed);
        if (repeat) SetKeyRepeating(key, true);
    }

    // Called from GLFW mouse callback
    void OnNativeMouseButton(int button, int action) {
        MouseButton btn = MyBackendKeyMap::NativeToMouseButton(button);
        SetMouseButtonDown(btn, action == GLFW_PRESS);
    }
};
```

---

### 6. Implement ImGuiBackend

> [!IMPORTANT]
> Install platform callbacks in `Init`, update `ImGuiIO` in `BeginFrame`, and flush draw data in `EndFrame`. See the [ImGuiBackend interface section](#imguibackend) for a full lifecycle example.

**Key points to verify:**

| Setting                      | Description                                                   |
| ---------------------------- | ------------------------------------------------------------- |
| `io.DisplaySize`             | Must be set in `BeginFrame` if the window can be resized.     |
| `io.DisplayFramebufferScale` | Also updated on resize.                                       |
| `io.DeltaTime`               | Must be set to elapsed time in **seconds**, not milliseconds. |
| Mouse position               | Must be in **window-space pixels**, not screen-space.         |

```cpp
class MyBackendImGuiBackend : public ImGuiBackend {
public:
    void Init(Window& window) override {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendPlatformName = "imgui_impl_mybackend";
        // ... install callbacks, init renderer ...
    }
    void Shutdown() override {
        // ... destroy GPU resources ...
        ImGui::DestroyContext();
    }
    void BeginFrame() override {
        // ... update io.DisplaySize, io.DeltaTime, cursor ...
        ImGui::NewFrame();
    }
    void EndFrame() override {
        ImGui::Render();
        // ... issue GPU draw calls for ImGui::GetDrawData() ...
    }
};
```

---

### 7. Implement FontRenderer

> [!NOTE]
> `Init` and `Shutdown` manage any shared GPU resources (e.g. a dedicated shader program for text rendering).

- `LoadFontAtlas`: Uploads the bitmap atlas to GPU memory and writes the resulting GPU texture ID back into `font.Atlas.TextureID`.
- `RenderText`: Computes per-glyph screen quads and issues batched draw calls.

```cpp
class MyBackendFontRenderer : public FontRenderer {
public:
    void Init() override;
    void Shutdown() override;
    void LoadFontAtlas(const Font& font) override;
    void UnloadFontAtlas(FontHandle handle) override;
    void RenderText(FontHandle handle,
                    const std::string& text,
                    float x, float y,
                    float fontSize,
                    float spacing,
                    const Color& color) override;
};
```

---

### 8. Wire Up BackendFactory

> [!IMPORTANT]
> Add one `#elif` branch per `Create*` function in `BackendFactory.cpp`. Each function follows the same structure — include the concrete header, return a `CreateScope<>` of the concrete type:

```cpp
Scope<RendererAPI> BackendFactory::CreateRenderer() {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibRendererAPI>();
    #elif defined(RS_BACKEND_MYBACKEND)
        return CreateScope<RS::Backend::MyBackendRendererAPI>();
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}

Scope<Window> BackendFactory::CreateAppWindow(const WindowProps& props) {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibWindow>(props);
    #elif defined(RS_BACKEND_MYBACKEND)
        return CreateScope<RS::Backend::MyBackendWindow>(props);
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}

Scope<Input> BackendFactory::CreateInput() {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibInput>();
    #elif defined(RS_BACKEND_MYBACKEND)
        return CreateScope<RS::Backend::MyBackendInput>();
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}

Scope<ImGuiBackend> BackendFactory::CreateImGuiBackend() {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibImGuiBackend>();
    #elif defined(RS_BACKEND_MYBACKEND)
        return CreateScope<RS::Backend::MyBackendImGuiBackend>();
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}

Scope<Fonts::FontRenderer> BackendFactory::CreateFontRenderer() {
    #if defined(RS_BACKEND_RAYLIB)
        return CreateScope<RS::Backend::RaylibFontRenderer>();
    #elif defined(RS_BACKEND_MYBACKEND)
        return CreateScope<RS::Backend::MyBackendFontRenderer>();
    #else
        RS_CORE_ASSERT(false, "No backend selected");
        return nullptr;
    #endif
}
```

> [!NOTE]
> Also update `BackendFactory.hpp` to forward-declare any new concrete classes that the factory must reference.

---

### 9. Update CMake Configuration

> [!IMPORTANT]
> Add a new branch in the root `CMakeLists.txt` and in `src/CMakeLists.txt`. The root file defines the macro and pulls in the backend's CMake module (which finds/fetches the required third-party library). The source file adds the backend's `.cpp` files to the `Raysim` target:

```cmake
# Root CMakeLists.txt
if(RS_BACKEND STREQUAL "raylib")
    add_compile_definitions(RS_BACKEND_RAYLIB)
    # ... existing raylib module ...
elseif(RS_BACKEND STREQUAL "mybackend")
    add_compile_definitions(RS_BACKEND_MYBACKEND)
    include(cmake/Backends/mybackend.cmake)   # finds/fetches the dependency
else()
    message(FATAL_ERROR "Unknown RS_BACKEND: '${RS_BACKEND}'. Valid values: raylib, mybackend")
endif()
```

```cmake
# src/CMakeLists.txt
if(RS_BACKEND STREQUAL "mybackend")
    target_sources(Raysim PRIVATE
        ${RS_BACKEND_ROOT}/MyBackendRendererAPI.cpp
        ${RS_BACKEND_ROOT}/MyBackendWindow.cpp
        ${RS_BACKEND_ROOT}/MyBackendInput.cpp
        ${RS_BACKEND_ROOT}/MyBackendImGuiBackend.cpp
        ${RS_BACKEND_ROOT}/MyBackendFontRenderer.cpp
    )
endif()
```

## Platform-Specific Considerations

### Win32

> [!WARNING]
> Windows introduces several macro collisions that affect backend code. Before including any Raysim or raylib header inside a backend `.cpp` or `.hpp` file, ensure the following defines are in place:

```cpp
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #undef DrawTextEx
    #undef DrawText
    #undef CreateWindow
#endif
```

> [!IMPORTANT]
> `DrawText` and `CreateWindow` are defined as macros by `<windows.h>` and clash with Raysim and raylib symbols of the same name. `NOMINMAX` prevents `<windows.h>` from defining `min`/`max` macros that collide with `std::min`/`std::max`.

<!-- -->

> [!NOTE]
> These guards are already applied in the engine's `Application.hpp` for user-facing code, but backend `.cpp` files that pull in both `<windows.h>` and raylib headers must apply them manually.

<!-- -->

> [!NOTE]
> `RS_DEBUGBREAK()` expands to `__debugbreak()` on MSVC, which signals the debugger without terminating the process.

---

### macOS

> [!CAUTION]
> macOS is **not officially tested**. The build system and vcpkg dependencies are cross-platform and should work in theory, but compatibility is not guaranteed.

**Known gaps:**

| Issue             | Description                                                                                                                                                                    |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| No Metal backend  | A Metal `RendererAPI` would need to be authored from scratch using the Metal C++ API or Objective-C++ wrappers.                                                                |
| `RS_DEBUGBREAK()` | Expands to `__builtin_trap()`, which terminates the process rather than pausing the debugger on some configurations. Use LLDB's breakpoint on `__builtin_trap` to catch these. |
| NSRunLoop         | Cocoa window creation (via GLFW or SDL2) requires the main thread to have a `NSRunLoop`. Raylib already handles this internally; custom backends must do the same.             |

---

### Linux

> [!NOTE]
> On Linux, `RS_DEBUGBREAK()` uses `__builtin_trap()`. Both X11 and Wayland display servers are viable:

| Display Server | `GetNativeWindow()` Returns | Context                           |
| -------------- | --------------------------- | --------------------------------- |
| **X11**        | `Xlib::Window` (`uint64_t`) | `GLXContext`                      |
| **Wayland**    | `wl_surface*`               | Requires `EGL` for OpenGL context |

> [!TIP]
> GLFW and SDL2 abstract these differences, which is one reason they are the **preferred choices** for new backends on Linux.

<p align="center">
  <img src="images/footer.png" alt="Raysim Footer" width="720" />
</p>
