# Raysim

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Early%20Development-yellow?style=flat)
[![Version](https://img.shields.io/badge/Version-0.3.1-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)

Raysim is a C++ framework for 2D graphics and interactive applications, built on top of [raylib](https://www.raylib.com/).

Inspired by **p5.js** and **Processing**, it provides a simple class-based API for drawing shapes, handling input, managing time, and running fixed-timestep simulations. Backend headers are never exposed to user code -- all access goes through clean abstract interfaces.

Useful for learning graphics programming, prototyping ideas, or building small games and simulations.

> **Note:** This project is in **early development**. The API may change without warning, and some features are still being implemented. It is my first time developing a C++ framework. Feedback and contributions are welcome!

## Quick Demos

<!-- Screenshots or GIF.
    TODO:
-->

<table>
  <tr>
    <td align="center">
      <img src="docs/images/bouncing-balls.png" width="400" alt="Bouncing Balls" /><br />
      <b>BouncingBalls</b>
    </td>
    <td align="center">
      <img src="docs/images/lissajous-curves.png" width="400" alt="Lissajous Curves" /><br />
      <b>LissajousCurves</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="docs/images/mouse-2d.png" width="400" alt="Mouse 2D" /><br />
      <b>Mouse2D</b>
    </td>
    <td align="center">
      <img src="docs/images/noise-landscape.png" width="400" alt="Noise Landscape" /><br />
      <b>NoiseLandscape</b>
    </td>
  </tr>
</table>

| Example           | Description                                                                             |
| ----------------- | --------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Physics simulation with gravity, mouse attraction/repulsion, and ball spawning.         |
| `LissajousCurves` | Parametric curve visualiser with animated phase shift and selectable frequency presets. |
| `Mouse2D`         | Mouse tracking and 2D interaction.                                                      |
| `NoiseLandscape`  | Procedurally generated scrolling terrain using various noise functions.                 |

### Using Makefile

```bash
make example-bouncing
make example-lissajous
make example-mouse
make example-noise
```

### Using CMake

To compile the examples, enable the `RAYSIM_BUILD_EXAMPLES` option when configuring with CMake:

```bash
cmake -B build -DRAYSIM_BUILD_EXAMPLES=ON
```

> Each example is a standalone executable in `examples/` that demonstrates different features of the framework. You can run them after building the project.

## Architecture

```txt
+──────────────────────────────────────+
|         User Application             |
|     (subclass of RS::Application)    |
+──────────────────────────────────────+
                   |
                   v
+──────────────────────────────────────+
|          RS::Application             |
|                                      |
|  Setup()         (once, on start)    |
|  Update(dt)      (every frame)       |
|  FixedUpdate(fdt)(fixed timestep)    |
|  Draw(alpha)     (every frame)       |
+──────────────────────────────────────+
        |             |            |
        v             v            v
   IRenderer       IWindow      IInput
        |             |            |
        +─────────────+────────────+
                      |
              Raylib Backend
         (RaylibRenderer, RaylibWindow,
              RaylibInput)
```

Supporting systems available from any lifecycle method:

- `RS::Time` - frame timing, fixed timestep accumulator, time scale, pause/resume
- `RS::Log` - structured logging via spdlog (console and log file)
- `RS::FontManager` - font loading and global access for text rendering
- `RS::Math::Random` - seeded RNG and procedural noise (Perlin 2D/3D, Simplex, Cellular, Value, fBm)

## Application Loop

Each application cycle runs four methods in order. Only `Draw` is mandatory.

<details>
<summary>Setup</summary>

Called once before the main loop starts. Use it to configure the window,
load assets, seed the RNG, and initialise your simulation state.

```cpp
void Setup() override {
    SetTitle("My Simulation");
    SetSize(1280, 720);
    SetRandomSeed(42);   // Optional: omit for a different seed every launch
    Time::SetTargetFPS(60);
}
```

</details>

<details>
<summary>Update</summary>

Called every frame. Use it for input polling, game logic, and anything that
reads or writes simulation state. Receives the scaled delta time in seconds
so movement stays frame-rate independent.

```cpp
void Update(float dt) override {
    if (Input->IsKeyPressed(Key::Space)) TogglePause();
    position += velocity * dt;
}
```

</details>

<details>
<summary>FixedUpdate</summary>

Called at a fixed timestep regardless of the actual frame rate. Use it for
physics integration and deterministic simulation steps. The accumulator runs
as many fixed steps as needed to catch up with real time, capped by
`maxFixedSteps` of `ApplicationConfig` to avoid a spiral of death.

```cpp
void FixedUpdate(float fixedDt) override {
    velocity += gravity * fixedDt;
    position += velocity * fixedDt;
}
```

</details>

<details>
<summary>Draw</summary>

Called every frame after `Update`. Issue all rendering commands here.
Receives an interpolation factor `alpha` in `[0, 1)` representing how far
the simulation has advanced into the next fixed step. Use it to lerp between
the previous and current physics snapshot so visuals stay smooth at any
frame rate. Do not mutate state inside `Draw`.

```cpp
void Draw(float alpha) override {
    Background(Color::Black());
    Vector2 renderPos = prevPosition + (position - prevPosition) * alpha;
    Shapes::DrawCircle(renderPos.x, renderPos.y, radius, Color::White());
}
```

> Using a separate Draw step is ideal because it keeps code structure clean and avoids inconsistencies with the physics loop. However, it’s optional. If you prefer, you can put all your rendering logic in Update and leave Draw empty. In that case, the `alpha` parameter will always be 0 since there’s no interpolation happening, but it won’t cause any issues.

</details>

## Modules

<details>
<summary>Core</summary>

| File                | Purpose                                                                                                                                                       |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Base class. Inherit from it, override lifecycle methods, access backend through `Renderer`, `Window`, and `Input`.                                            |
| `ApplicationConfig` | Configure title, resolution, max fixed steps, and log file before the loop starts. All fields have defaults - pass only what you need.                        |
| `Time`              | Static utility. Delta time, fixed timestep, time scale, pause/resume, FPS counters.                                                                           |
| `Log`               | Wraps spdlog. Writes to console and a log file. Use macros `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`.                                                      |
| `FontManager`       | Load a TTF/OTF font once, access it globally for text rendering. You can set a default font using `SetDefaultFont("path/to/font.ttf")` on `Setup() override`. |
| `BackendFactory`    | Creates concrete `IRenderer`, `IWindow`, and `IInput` instances for the selected backend.                                                                     |

</details>

<details>
<summary>Graphics</summary>

| File         | Purpose                                                                                                                                                                                          |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `Shapes`     | Filled and outlined: rectangles, circles, lines, triangles. Each function accepts an optional `OriginMode` to anchor the shape at its center or any corner/edge instead of the default top-left. |
| `Texts`      | Draw text strings using the loaded font.                                                                                                                                                         |
| `Color`      | RGBA color struct `{r, g, b, a}` with HSV conversion. Build any color inline: `Color{255, 100, 0}`.                                                                                              |
| `Colors`     | Namespace of `constexpr` presets: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayBlack`, and more. Use these instead of constructing colors by hand.          |
| `OriginMode` | Enum used by Shapes to control the anchor point of a shape (TopLeft, Center, BottomRight, etc.).                                                                                                 |

</details>

<details>
<summary>Math</summary>

| File      | Purpose                                                                                                                                                                                                                                                                                                     |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Vector2` | 2D vector with arithmetic operators and common utility methods.                                                                                                                                                                                                                                             |
| `Vector3` | 3D vector, used internally for color/clear operations and general math.                                                                                                                                                                                                                                     |
| `Math`    | Common math helpers: clamp, lerp, map, wrap, and trigonometric utilities.                                                                                                                                                                                                                                   |
| `Random`  | Seeded Mersenne Twister RNG. Integer and float ranges, boolean helpers, container sampling, plus coherent noise (Perlin 2D/3D, Simplex, Cellular, Value) and Fractal Brownian Motion. Seed is auto-random on startup; call `Seed()` for deterministic results or `SeedRandom()` to re-randomise at runtime. |

</details>

<details>
<summary>Interfaces and Backend</summary>

The three abstract interfaces decouple user code from the underlying library:

| Interface   | Responsibility                                    |
| ----------- | ------------------------------------------------- |
| `IRenderer` | Frame begin/end, screen clear, VSync control.     |
| `IWindow`   | Title, size, fullscreen toggle, aspect ratio.     |
| `IInput`    | Keyboard, mouse buttons, cursor position, scroll. |

The `Raylib` backend is the only implementation included. `RaylibRenderer`,
`RaylibWindow`, and `RaylibInput` satisfy each interface. All raylib-specific
headers are confined to this layer and never leak into user code.

</details>

## Building

Minimum Requirements: **CMake 3.19** and **C++17**.
Raylib and spdlog are fetched automatically via CMake FetchContent on first configure.

```bash
make build                     # configure and build (Debug by default)
make build BUILD_TYPE=Release  # release build
make rebuild                   # clean then build
make help                      # list all available targets
```

Or with CMake directly:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DRAYSIM_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

## Quick Start

`ApplicationConfig` is optional - all fields default to basic values. You can choose to provide only what you need or define individual settings in `Setup` instead. The following are all valid ways to configure your app:

```cpp
// No config: uses defaults (1600x900, title "Raysim App")
MyApp app;

// Partial config with designated initializers (C++20 required)
MyApp app(ApplicationConfig{ .title = "My Sim", .width = 1280, .height = 720 });

// Full config
MyApp app(ApplicationConfig{
    .title         = "My Sim",
    .width         = 1920,
    .height        = 1080,
    .maxFixedSteps = 8,
    .logFile       = "mysim.log"
});
```

```cpp
#include <Raysim/Raysim.hpp>
using namespace RS;

class MyApp : public Application {
    Vector2 position = {400, 300};
    Vector2 velocity = {150, 100};

protected:
    void Setup() override {
        SetTitle("My First Raysim App");
        SetSize(800, 600);
        // Seed is auto-random on startup; call SetRandomSeed() only if you need reproducibility.
        // SetRandomSeed(12345);
        Time::SetTargetFPS(60);
    }

    void FixedUpdate(float fixedDt) override {
        position += velocity * fixedDt;

        if (position.x < 20 || position.x > GetWidth() - 20)  velocity.x *= -1;
        if (position.y < 20 || position.y > GetHeight() - 20) velocity.y *= -1;
    }

    void Draw(float /*alpha*/) override {
        Background(Colors::DarkBlue);
        Shapes::DrawCircle(position.x, position.y, 20.0f, Colors::RayWhite);
    }
};

int main() {
    MyApp app;
    app.Run();
    return 0;
}
```

## License

This project is licensed under the **Apache License 2.0**.
See the [LICENSE](LICENSE) file for details.
