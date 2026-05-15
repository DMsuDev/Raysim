<p align="center">
  <img src="docs/images/banner.png" alt="Raysim Banner" width="720" />
</p>

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Beta-blue?style=flat)
[![Version](https://img.shields.io/badge/Version-0.93.0-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)
[![License](https://img.shields.io/badge/License-Apache%202.0-lightgrey?style=flat)](LICENSE)

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)
• [Changelog](docs/CHANGELOG.md)
• [Architecture](docs/ARCHITECTURE.md)

Raysim is a C++ framework for 2D graphics and interactive applications, built on top of [raylib](https://www.raylib.com/).

Architecturally inspired by [**The Cherno's Hazel Engine**](https://github.com/TheCherno/Hazel), it provides a clean, class-based API for drawing shapes, handling input, managing time, and running fixed-timestep simulations.

Useful for learning graphics programming, prototyping ideas, or building small games and simulations.

Feedback and contributions are welcome. ❤️

> [!NOTE]
> This project is in **Beta**. The core API is stable but may still evolve. Some features are still being implemented.

## Quick Demos

<p align="center">
  <img src="docs/gif/Mouse2D.gif" alt="Mouse2D" width="720" /><br />
  <b>Mouse2D</b>
</p>

<p align="center">
  <img src="docs/gif/NoiseLandscape.gif" alt="NoiseLandscape" width="720" /><br />
  <b>NoiseLandscape</b>
</p>

<p align="center">
  <img src="docs/gif/SceneShowcase.gif" alt="SceneShowcase" width="720" /><br />
  <b>SceneShowcase</b>
</p>

| Example           | Description                                                                                                                                                                                       |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Physics simulation with gravity, mouse attraction/repulsion, and ball spawning. Spawn balls with left click, toggle gravity with G, and watch them collide and bounce against the walls.          |
| `LissajousCurves` | Parametric curve visualiser with animated phase shift and selectable frequency presets. The curve evolves in real time; try switching presets with the ImGui panel to see the patterns transform. |
| `Mouse2D`         | Tracks the mouse position with smooth exponential interpolation and draws a trailing circle. A good starting point to understand the input and rendering pipeline.                                |
| `NoiseLandscape`  | Procedurally generated scrolling terrain rendered in layered passes using Perlin, Simplex, Cellular, and Value noise. Press R to reseed all layers and get a completely new landscape.            |
| `SceneShowcase`   | Three independent visual scenes (StarField, PlasmaArt, ClockMandala) accessible from a shared ImGui navigation overlay. Shows how to switch scenes at runtime using the SceneManager.             |

All examples are built alongside the library. See the [Building](#-building) section for setup instructions.

> [!TIP]
> Looking for the full module reference, lifecycle callbacks, and event system details? See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## 🔧 Building

For detailed system requirements, supported platforms, and dependency information, see the [System & Build Requirements](docs/REQUIREMENTS.md) document.

> [!NOTE]
> **macOS** support has not been officially tested. The build system and dependencies should work in theory, but compatibility is not guaranteed.

### 1. Clone with submodules

vcpkg is included as a Git submodule, so you need to initialise it after cloning. If you cloned the repo without `--recurse-submodules`, run:

```bash
git submodule update --init --recursive
```

### 2. Setup

Once the submodule is ready, the setup script bootstraps vcpkg and installs all required dependencies automatically:

```bash
./tools/setup_all.sh       # Linux
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

The `debug` preset enables ASan and UBSan by default.

> [!WARNING]
> Sanitizer support is experimental and may not work on all toolchains. On MinGW they are automatically disabled.

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

## 🚀 Quick Start

> [!TIP]
> The example below is intentionally minimal. For a deeper look at the scene lifecycle, layers, events, and all available modules, see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/EXAMPLES.md](docs/EXAMPLES.md).

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
  <img src="docs/images/footer.png" alt="Raysim Footer" width="720" />
</p>
