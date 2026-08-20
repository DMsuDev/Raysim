<p align="center">
  <img src="docs/images/banner.png" alt="Raysim Banner" width="720" />
</p>

<div align="center">

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Beta-blue?style=flat)
[![CI Build](https://github.com/DMsuDev/Raysim/actions/workflows/ci-build.yml/badge.svg)](https://github.com/DMsuDev/Raysim/actions/workflows/ci-build.yml)
[![Version](https://img.shields.io/github/v/release/DMsuDev/RaySim?style=flat&label=Version&color=brightgreen)](https://github.com/DMsuDev/RaySim/releases/latest)
[![License Apache](https://img.shields.io/github/license/DMsuDev/RaySim?style=flat&label=License&color=lightgrey)](LICENSE)

</div>

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)
• [Changelog](docs/CHANGELOG.md)
• [Architecture](docs/ARCHITECTURE.md)

---

**Raysim** is an experimental C++ framework for 2D graphics and interactive applications, built on top of [raylib](https://www.raylib.com/) and inspired by the architecture of [**The Cherno's Hazel Engine**](https://github.com/TheCherno/Hazel).

It started as a personal learning project to explore game engine design patterns, abstract rendering pipelines, and deepen modern C++ development skills.

It provides an object-oriented abstraction layer that simplifies window creation, input handling, scene state management, and fixed-timestep loop control. Its goal is to offer a ready-to-use foundation for prototyping ideas, simulating physics, or developing small games without wasting time setting up basic infrastructure.

Feedback and contributions are welcome. ❤️

> [!NOTE]
> This project is in **Beta** and under active development. The API continuously evolves as best practices are adopted.

## Gallery

<table>
  <tr>
    <td width="240" align="center">
      <img src="docs/gif/Mouse2D.gif" alt="Mouse2D" width="100%" />
    </td>
    <td valign="top">

**Mouse2D**

Tracks the mouse with smooth exponential interpolation and draws a trailing circle. Good entry point for understanding the input and rendering pipeline.

`input` `interpolation` `rendering`

</td>
  </tr>
  <tr>
    <td width="240" align="center">
      <img src="docs/gif/NoiseLandscape.gif" alt="NoiseLandscape" width="100%" />
    </td>
    <td valign="top">

**NoiseLandscape**

Procedurally generated scrolling terrain rendered in layered passes using Perlin, Simplex, Cellular, and Value noise. Press `R` to reseed all layers.

`procedural` `noise` `perlin`

</td>
  </tr>
  <tr>
    <td width="240" align="center">
      <img src="docs/gif/SceneShowcase.gif" alt="SceneShowcase" width="100%" />
    </td>
    <td valign="top">

**SceneShowcase**

Three independent visual scenes (`StarField`, `PlasmaArt`, `ClockMandala`) accessible from a shared ImGui navigation overlay. Shows dynamic runtime scene switching using the `SceneManager`.

`scene-manager` `imgui` `runtime`

</td>
  </tr>
</table>

## Requirements

| Tool             | Version | Notes                                             |
| :--------------- | :------ | :------------------------------------------------ |
| **CMake**        | 3.28+   | Build system generator                            |
| **C++ Compiler** | C++20   | GCC 11+, Clang 12+ (Linux) · MSVC 2022+ (Windows) |
| **Ninja**        | Any     | Recommended build generator                       |

## Building

Raysim officially supports:

- **Linux** (Debian/Ubuntu recommended)
- **Windows 10/11**
- **macOS** (not officially tested; expected to work in theory)

### 1. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/DMsuDev/raysim.git
cd raysim
```

If you already cloned without submodules, run:

```bash
git submodule update --init --recursive
```

### 2. Build

<details open>
<summary><strong>Option A: CMake Presets (Recommended)</strong></summary>

Presets handle configuration, and sanitizer flags automatically:

**Linux / macOS**

```bash
# Debug build (ASan + UBSan enabled where supported)
cmake --preset debug
cmake --build --preset debug

# Release build
cmake --preset release
cmake --build --preset release
```

**Windows (MSVC)**

```bash
# Debug build
cmake --preset msvc-debug
cmake --build --preset msvc-debug

# Release build
cmake --preset msvc-release
cmake --build --preset msvc-release
```

> [!NOTE]
> Sanitizer support varies by platform and toolchain. On Windows, only ASan is available with Clang (non-cl). MinGW and MSVC sanitizers are not yet supported.

</details>

<details>
<summary><strong>Option B: Custom Build</strong></summary>

If you prefer explicit control or aren't using presets, you can configure and build manually.

**Linux / macOS / Windows (Ninja Generator)**

```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DRS_BUILD_EXAMPLES=ON

cmake --build build
```

**Windows (PowerShell + Visual Studio)**

```powershell
cmake -B build -G "Visual Studio 18 2026" -A x64 `
  -DRS_BUILD_EXAMPLES=ON

cmake --build build --config Release
```

</details>

## Quick Start

A Raysim application is built around **scenes**. Each scene manages its own lifecycle, rendering, and input: register it with `Application`, activate it, and the engine handles the rest.

The `RS_SCENE` macro registers the type with the scene system and generates its constructor automatically. Override only the callbacks you need: all are no-ops by default.

| Callback                 | Execution Timing                                                         |
| :----------------------- | :----------------------------------------------------------------------- |
| `OnAttach`               | Once, on scene push. Ideal for loading assets and subscribing to events. |
| `OnStart`                | Every time the scene becomes active. Use this to reset state.            |
| `OnUpdate(dt)`           | Every frame. Input polling and variable-rate logic.                      |
| `OnFixedUpdate(fixedDt)` | Fixed timestep. Physics and deterministic simulation.                    |
| `OnDraw(alpha)`          | Every frame, after `OnUpdate`. All rendering logic belongs here.         |
| `OnDetach`               | On scene removal. Release resources and unsubscribe from events.         |

For the full lifecycle reference including events, layers, and physics interpolation, see [Architecture](docs/ARCHITECTURE.md).

The example below creates a bouncing ball that reflects off the window bounds:

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

        float width  = static_cast<float>(window().GetWidth());
        float height = static_cast<float>(window().GetHeight());

        if (position.x < 20 || position.x > width  - 20) velocity.x *= -1;
        if (position.y < 20 || position.y > height - 20) velocity.y *= -1;
    }

    void OnDraw(float /*alpha*/) override {
        renderer().ClearScreen(Colors::DarkBlue);
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

Physics logic lives in `OnFixedUpdate`: the engine executes it at a fixed timestep independent of the frame rate. `OnDraw` receives an `alpha` interpolation factor for smooth rendering between physics steps; this example omits interpolation for brevity.

For in-depth usage of every module (input, events, layers, math, ImGui), see the [GUIDE.md](docs/GUIDE.md). For runnable demos with full source code, see the [`examples/`](examples/) directory.

## Contributing

Contributions are always welcome! ❤️ Whether you are reporting bugs, fixing issues, adding new examples, or improving the documentation, your help is appreciated.

Before opening a pull request:

- Keep pull requests focused: prefer small, atomic PRs that address a single feature or fix.
- Write clear commit messages using [Conventional Commits](https://www.conventionalcommits.org/).
- Ensure the project builds cleanly without introducing new compiler warnings.

For major changes or new features, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the **Apache License 2.0**.
See the [LICENSE](LICENSE) file for details.

<p align="center">
  <img src="docs/images/footer.png" alt="Raysim Footer" width="720" />
</p>
