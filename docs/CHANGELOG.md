# Changelog

All notable changes to the **Raysim** project will be documented in this file.

## [0.95.0] - 2026-08-20

### Breaking Changes

- **Submodule Initialization Required**: `vcpkg` is no longer supported. Existing local setups must initialize submodules via `git submodule update --init --recursive`.
- **Toolchain Updates**: Custom scripts referencing `vcpkg.cmake` or vcpkg toolchains must be updated to target standard CMake CMakeLists.

### Added

- **Git Submodules Migration**: Replaced `vcpkg` with Git submodules (`third_party/raylib`) for managing dependencies, eliminating external package manager requirements.
- **Unified Unix Preset**: Grouped Linux and macOS build configurations under a single `unix` CMake preset.
- **Engine Version Header**: Added auto-generated `Version.hpp` for querying version info at compile time.

### Changed

- **Build System & Compiler Modules**: Simplified CMake architecture, utilizing native CMake Interprocedural Optimization (IPO) for LTO, isolated target scoping for sanitizers, and streamlined coverage flags.
- **Backend Architecture**: Modernized `BackendRegistry` and backend modules (`cmake/Backends/`) with self-contained dependency resolution.
- **CI/CD Pipelines**: Updated GitHub Actions workflows for submodule checkouts, SHA-pinned actions, and cleaned up Dependabot rules.

### Removed

- Removed `vcpkg` submodules, update workflows, and legacy setup scripts (`tools/`).
- Removed `clang-tidy` config, `REQUIREMENTS.md`, unused presets (`base-features`), and redundant CMake status prints.

## [0.94.2] - 2026-07-15

### Added

- **Cross-Platform Build Tooling Suite**: Added unified scripts in `tools/` for building, cleaning, validating dependencies, and running examples across Windows and Unix-like systems.

- **EditorConfig & Git Attributes**: Added `.editorconfig` to enforce coding style standards and `.gitattributes` to normalize line endings and configure `export-ignore` for third-party files.

- **macOS Platform Support**: Added experimental platform detection and compile-time validation for macOS in `include/Raysim/Core/PlatformDetection.hpp`.

- **DLL Export/Import Visibility (`RS_API`)**: Applied `RS_API` macro to public classes and configured ImGui DLL integration for consistent Windows dynamic library symbol exports (`__declspec(dllexport)` / `__declspec(dllimport)`).

- **Math Utilities**: Extended `Math::Pow` with proper type handling and optimized integer exponentiation routines.

### Changed

- **Application Lifecycle Management**: Enhanced deterministic destruction sequence in `EntryPoint.hpp` and profiling sessions to guarantee proper subsystem cleanup before process exit.

- **Build Tooling & Dependencies**: Unified setup workflows under `setup_all`, updated `vcpkg.json` baseline, and improved `init_vcpkg.ps1` error handling.

- **Platform & Core Utilities**: Refactored `PlatformDetection.hpp` error diagnostic messages and expanded inline API documentation in `Base.hpp`.

- **CMake & Build Settings**: Streamlined linker behavior for Raylib/GLFW backends and silenced duplicate console warnings in `cmake/Sanitizers.cmake`.

- **Examples & Pre-commit Hooks**: Updated `.pre-commit-config.yaml` rules for third-party directory exclusions, and refactored `NoiseLandscape` variable names (`skyH` → `skyHeight`).

### Fixed

- **Assert Macro Naming**: Corrected symbol identifier mismatch, standardizing `RS_STRINGIFY_MACRO` to `RS_STRINGIFY`.

- **Mouse Grid Detection**: Fixed grid movement alignment offsets in `examples/Mouse2D` for precise visual cell tracking.

- **Shell Script Portability**: Fixed file permissions, line-ending mismatches (CRLF to LF), and shell compatibility across Unix systems.

## [0.94.1] - 2026-06-20

### Breaking Changes & API Updates

- **Scene Accessors Migration**: Standardized all `Scene` subsystem accessors to camelCase, replacing legacy PascalCase methods and adding direct access to the `EventBus`.
  - Replaced `GetInput()` → `input()`, `GetWindow()` → `window()`, `GetSceneManager()` → `sceneManager()`, and `GetRenderer()` → `renderer()`.
  - Added new `eventBus()` accessor.

- **Text API Simplification**: Removed `RenderTextInRectangle()` from `include/Raysim/Graphics/Texts.hpp` in preparation for a dedicated UI subsystem layout model.

- **RenderCommand Overloads**: Removed redundant `ClearScreen(Math::Vec3f)` overload in favor of explicit `Color` parameter passing.

### Added

- **Application FPS & VSync Controls**: Extended `ApplicationConfig` with startup frame timing options (`TargetFPS` defaults to `60`, `0` for uncapped; `VSync` defaults to `true`).

  ```cpp
  RS::ApplicationConfig config;
  config.TargetFPS = 144; // Set target FPS
  config.VSync = false;  // Enable or disable VSync
  ```

- **Modular Convenience Headers**: Added top-level module headers (`Events.hpp`, `Fonts.hpp`, `Graphics.hpp`, `Inputs.hpp`, `Layers.hpp`, `Math.hpp`, `Scenes.hpp`) aggregated under `Raysim.hpp` for streamlined include management.

- **Mathematical Utilities**: Extended `MathUtils.hpp` with proper interval wrapping, true mathematical modulus, and additional algebraic helpers.

  ```cpp
  float wrapped = Math::Wrap(val, -180.0f, 180.0f); // Half-open interval [min, max)
  float modVal  = Math::Mod(-5.0f, 3.0f); // Returns 1.0f (unlike C% operator which can return negative)
  float powVal  = Math::Pow(2.0f, 8.0f);
  ```

- **Engine & Subsystem Documentation**: Added centralized architecture and platform specifications:
  - `docs/ARCHITECTURE.md` (and Spanish translation `ARCHITECTURE.es.md`).
  - `docs/BACKENDS.md` (Raylib, GLFW/OpenGL, SFML, SDL2 platform details).
  - `docs/EXAMPLES.md` and `docs/REQUIREMENTS.md`.

- **Modularized Example Projects**: Restructured example sources into clear module boundaries:
  - **BouncingBalls**: Split into `example_ball.cpp/hpp` and `example_helpers.cpp/hpp`.
  - **LissajousCurves**: Separated simulation state into `LissajousSimulation.cpp/hpp` and `LissajousTypes.hpp`.
  - **Mouse2D**: Extracted UI layer implementation into `example_layers.cpp/hpp` and added grid movement visualization.

### Changed

- **Application Exception Boundaries**: Wrapped initialization, main loop execution (`Run()`), and shutdown inside `src/Core/Application.cpp` with `try/catch` boundaries that log context before rethrowing to `EntryPoint`.

- **Build System & Presets**: Modernized CMake workflow files, adding `RDynamic.cmake` for Windows symbol exports, improving `StandardSettings.cmake` (postfix and position-independent code support), and upgrading CMake presets to version 6.

- **Examples Codebase Alignment**: Updated all showcase applications to use new camelCase accessors (`input()`, `window()`, `renderer()`, etc.) alongside `Math::Vec2f` / `Math::Vec2i` types.

### Fixed

- **Shell Compatibility**: Fixed `tools/init_vcpkg.sh` execution under non-Bash shells (`sh`, `dash`) using `$0` fallbacks, and removed invalid non-breaking space characters causing Linux syntax errors.

- **Font Provider Safety**: Added validation for `fread()` return values in `STBTrueTypeProvider.cpp` to prevent partial buffer reads.

- **Raylib Font Buffer Allocation**: Fixed pixel array size calculation in `RaylibRendererAPI::LoadFont()`.

- **Header Inclusion Guards**: Added missing `<cstdint>` header inclusion in `include/Raysim/Core/LayerStack.hpp`.

- **CMake Target Configuration**: Corrected include directory syntax for target `Raysim` in root `CMakeLists.txt`.

- **CI Diagnostics**: Updated `.github/workflows/codeql.yml` to filter out `third_party/` and `vcpkg` directory paths.

## [0.93.0] - 2026-05-12

### Breaking Changes & API Updates

- **Typed Vector Aliases**: `Math::Vec2f` (`Vector2<float>`) and `Math::Vec2i` (`Vector2<int>`) replace the previous untyped `Math::Vec2` usage throughout the codebase and public API.

- **Header Precompilation**: Renamed `pch.hpp` to `rspch.hpp` across internal sources to eliminate global inclusion collisions.

### Added

- **Top-Level Exception Guard**: The generated `main()` now protects the complete application lifecycle within nested `try/catch` blocks, logging critical unhandled exceptions and safely flushing logs via `Log::Shutdown()` prior to exit.

- **Exception Assertions**: Added `RS_CORE_ASSERT_THROW()` and `RS_ASSERT_THROW()` to capture and log source location metadata at `CRITICAL` level when expressions throw exceptions before rethrowing. Compiles to `((void)0)` when assertions are disabled.

  ```cpp
  // Engine-internal use (core logger)
  RS_CORE_ASSERT_THROW(m_Window->Init(spec));

  // Application use (client logger)
  RS_ASSERT_THROW(renderer.LoadShader("default.glsl"));
  ```

- **Log File Truncation**: Extended `Log::Init()` with `LogFileMode` support. Passing `LogFileMode::Truncate` clears the active log file on startup for clean single-session debugging.

  ```cpp
  Log::Init(/*async=*/true, LogFileMode::Truncate);
  ```

- **EventBus Architecture**: Added a type-safe publish/subscribe event bus owned by `Application` and exposed via `EngineContext::Bus` for global broadcast events.

  ```cpp
  // Subscribe in OnAttach
  m_SubID = GetContext().Bus->Subscribe<KeyPressedEvent>([](const KeyPressedEvent& e) {
      RS_LOG_INFO("Key pressed: {}", static_cast<int>(e.GetKeyCode()));
  });

  // Unsubscribe in OnDetach
  GetContext().Bus->Unsubscribe(m_SubID);
  ```

- **Event-Driven Input Abstraction**: Introduced `EventDrivenInput` as an intermediate base class for event-driven backends (GLFW, SDL2), maintaining internal state arrays (Down/Pressed/Released/Repeating) via `OnEvent()`.

- **Gamepad & Axis Definitions**: Added backend-agnostic `GamepadButton` and `GamepadAxis` enumerations using Xbox-style layout conventions.

- **Strongly Typed Angle Abstraction**: Added the `Angle` type to eliminate unit ambiguity between degrees and radians, complete with literal operators, arithmetic, and range wrapping.

  ```cpp
  using namespace RS::Math::Literals;

  Angle angle = 90.0_deg;
  Angle rad = Angle::Radians(1.5707f);
  Angle sum = (angle + rad).WrapSigned(); // Wrapped to (-180°, 180°]
  ```

- **Vector Rotation & Math Utilities**: Added `Vector2::RotatedBy(Angle)` along with helper utilities in `MathUtils.hpp` (`SmoothDamp`, `MoveTowards`, `EpsilonEquals`, `Saturate`, `Fract`).

### Changed

- **EngineContext Expansion**: Exposed `EventBus* Bus` through `EngineContext`, enabling scenes and layers to emit or listen to events without direct `Application` dependencies.
- **Input System Capabilities**: Extended `Input` queries to support gamepads (`IsGamepadButtonDown`, `GetGamepadAxis`, `IsGamepadConnected`).
- **Raylib Event Feeding**: Refactored Raylib backend callbacks to push OS and window events directly into the `EventDrivenInput` pipeline.
- **Color Rework**: Improved `Color` module with inline implementations, better documentation, and a cleaner API.
- **Third-Party Dependency Strategy**: Moved `imgui` and `spdlog` out of git submodules into local `third_party/` source targets.
- **Examples Migration**: Updated all showcase applications (`BouncingBalls`, `NoiseLandscape`, `SceneShowcase`, etc.) to use `Math::Vec2f` and `Math::Vec2i`.

### Fixed

- **Dynamic Linking**: Fixed DLL symbol visibility and linkage errors associated with `Scene::GetRenderer()`.
- **CMake Build Overhead**: Disabled implicit C++20 module scanning in CMake configuration to reduce build times and prevent generator conflicts.
- **Toolchain Diagnostics**: Extended `Sanitizers.cmake` to support MSVC sanitizer flag configurations.

## [0.91.1] - 2026-05-08

### Changed

- **Sanitizers**: Improved sanitizer and MSVC compatibility, including handling for compiler flag overrides.

### Fixed

- **MSVC Build & Linker Compatibility**: Conditionalized `$<LINK_GROUP:RESCAN>` generator expressions in `rs_windowing_raylib` CMake configuration. MSVC toolchains now link `raylib` and `glfw` directly to bypass GNU-specific linker flag errors (`--start-group`).

- **MSVC Warnings**:
  - Fixed warning `D9025` caused by conflicting warning-level options.
  - Fixed warning `C4996` for `fopen` in `STBTrueTypeProvider.cpp`.
  - Fixed warning `C4100` for the unused `window` parameter in `RaylibImGuiBackend::Init`.

## [0.91.0] - 2026-05-07

### Breaking Changes

- **Math API Restructuring**: Renamed and reorganized vector types under `Math/Types/`. Replaced `Vector2` and `Vector3` with `Math::Vec2` and `Math::Vec3`. Added integer variants `Vec2i` and `Vec3i`.

- **Scene Lifecycle & API Overhaul**:
  - Deprecated legacy `AddScene()` and `SetInitialScene()` methods in favor of `RegisterScene<T>()` and `ChangeScene()`.
  - Replaced by-value `EngineContext` storage within scenes with a lightweight `EngineContext*` reference.

- **Window Architecture & NVI Pattern**:
  - Refactored `Window` to follow the Non-Virtual Interface (NVI) pattern. Virtual implementations transitioned to protected `Impl*` calls (`ImplPollEvents`, `ImplSwapBuffers`, `ImplSetSize`, etc.).
  - Replaced implicit `Window::OnUpdate()` abstraction with explicit `PollEvents()` and `SwapBuffers()` operations.
  - Standardized vector properties in `Window` to use `Math::Vec2i`.

- **Application Centralized Configuration**:
  - `ApplicationConfig` is now the standard entry point for initial application setup. `config.Window.Title`/`Width`/`Height` replaces post-initialization calls like `GetWindow().SetTitle()` or `GetWindow().SetSize()`.

- **API Method Renames**:
  - `Text::DrawText` renamed to `Text::RenderText`.
  - `BackendFactory::CreateWindow` renamed to `CreateAppWindow`.

### Added

- **Font Subsystem (`RS::Fonts::FontManager`)**:
  - Modular `FontManager` orchestrating `TrueTypeProvider` (STB backend) and GPU `FontRenderer` (Raylib backend).
  - Handle-based asset caching (`FontHandle`), font lookup mapping (`s_NameMap`), and custom TTF/OTF atlas generation via `stb_truetype`.
  - `MeasureText()` utility supporting tight bounding-box calculation using glyph metrics and custom codepoint ranges (`firstChar`, `charCount`).
  - Integrated built-in default font registration via `LoadDefaultFont()`.

- **Macro-Based Scene System (`RS_SCENE`) & RTTI**:
  - Introduced `RS_SCENE(Type)` macro for automatic static ID generation (`StaticID()`), type metadata, and constructor binding.
  - Implemented Compile-time/Runtime type ID generator (`SceneID`, `GetSceneTypeID<T>()`).

- **Layer System Architecture (`Layer` & `LayerStack`)**:
  - Abstract `Layer` base class introducing dedicated `OnUpdate(dt)` for logic and `OnUIRender()` for isolated ImGui UI rendering.
  - Layer lifecycle callbacks (`OnAttach()`, `OnDetach()`) and debug naming tracking via `m_DebugName` with `[[nodiscard]] GetName()`.
  - Integrated `LayerStack` flow control abstractions within scenes (`PushLayer`, `PushOverlay`, `PopLayer`, `PopOverlay`).
  - Added native `ImGuiLayer` overlay support powered by `rlImGui` and `RaylibImGuiBackend`.
  - Added `ImGuiStyles` for centralized UI theme configuration.

- **Profiling Infrastructure (`RS::Instrumentor`)**:
  - Thread-safe `Instrumentor` infrastructure and RAII `InstrumentationTimer` producing Chrome Tracing Format (`chrome://tracing`) JSON sessions.
  - Compile-time `constexpr` signature sanitization (`CleanupOutputString`) to clean calling conventions (e.g., stripping `__cdecl`) and quote escaping for JSON safety.
  - Cross-compiler signature mapping (`RS_FUNC_SIG`) supporting MSVC (`__FUNCSIG__`), GCC/Clang (`__PRETTY_FUNCTION__`), and fallbacks.
  - Zero-overhead release macros (`RS_PROFILE_FUNCTION()`, `RS_PROFILE_SCOPE()`, `RS_PROFILE_BEGIN_SESSION()`, `RS_PROFILE_END_SESSION()`) controlled via `RS_ENABLE_PROFILE`.

- **Path Utilities (`PathUtils`)**: Platform-agnostic path resolution helpers for executable directory, working directory, and assets folder paths.

- **Logging Management**: Added `LogFileMode` enum (append vs. overwrite mode on startup) and support for TTL-based and unique log level registries.

- **Scene Showcase Example**: Added `SceneShowcase` multi-scene demo featuring `StarField`, `PlasmaArt`, and `ClockMandala` with real-time ImGui navigation.

- **Environment Tooling**: Added setup scripts for `vcpkg` initialization, Python virtual environment configuration with pre-commit hooks, and GitHub Actions workflow for automated `vcpkg` baseline updates.

### Changed

- **Scene Architecture (NVI Pattern & Macro System)**:
  - Redesigned `Scene` execution interface using the Non-Virtual Interface (NVI) pattern (`Start`, `Update`, `FixedUpdate`, `Draw`, `Pause`, `Resume`, `Stop`).
  - Integrated `RS_SCENE(Type)` macro to automate compile-time static type ID generation (`StaticID()`) and type reflection (`StaticName()`).
  - Added direct subsystem getter `GetSceneManager()` inside `Scene` for seamless inline scene transitions (`GetSceneManager().ChangeScene(...)`).
  - Integrated `LayerStack` lifecycle updating (`UpdateAll`) and ImGui frame dispatching (`ImGuiBeginFrame`, `RenderUIAll`, `ImGuiEndFrame`) directly into `Scene::Draw()`.

- **RenderCommand & Instrumentation**:
  - Added `Instrumentor` profiling markers (`RS_PROFILE_FUNCTION()`) to critical engine subsystems and static `RenderCommand` entry points.
  - Standardized proxy accessor `GetRenderer()` in `Scene` returning static `RenderCommand` references for backward compatibility.

- **Time Infrastructure**: Refactored delta time calculations to support unscaled smooth delta time tracking.

- **Window API & NVI Architecture**:
  - Added NVI guards (`RS_PROFILE_FUNCTION()`, `RS_CORE_ASSERT`) directly in `Window` wrapper methods.
  - Added `GetFramebufferSize()` and template-based `GetNativeWindow<T>()` casting helper.

- **Core Memory & Utility Consolidation**: Centralized memory management smart pointer aliases (`Scope`, `Ref`) and assertion macros into `Base.hpp`.

- **Math Utilities**: Introduced `MathUtils.hpp` for mathematical constants and helper functions. Standardized min/max functions to `Math::MaxValue` and `Math::MinValue`.

### Fixed

- **Win32 & Header Conflicts**: Isolated Win32 definition exclusions (`WIN32_LEAN_AND_MEAN`) across system headers to prevent redefinition errors.

- **Precompiled Headers**: Fixed relative include paths to ensure direct references for PCH compilation across build backends.

- **Logging Shutdown**: Resolved deadlock and thread shutdown issues in `Log::Shutdown()`.

- **Workflows**: Set `fetch-depth: 0` in GitHub Actions workflows to fix shallow cloning errors during `vcpkg` ports extraction.

### Build & Infrastructure

- **Backend Build Refactoring**:
  - Introduced modular CMake backend system via `RS_BACKEND` cache option (`raylib` [default], `glfw_opengl`, `sfml`, `sdl2`).
  - Added `BackendRegistry.cmake` to dynamically load `cmake/Backends/<name>.cmake` configurations and link against a unified `rs_backend` meta-target.

- **CMake Architecture Modernization**:
  - Modularized build options into `rs_options`: split logic into `CompilerWarnings.cmake`, `CompilerOptimization.cmake`, and `Sanitizers.cmake`.
  - Added explicit Link-Time Optimization (`LTO`) support and runtime sanitizers integration.
  - Disabled shared library options by default to ensure static linkage consistency.

- **Dependency Management**:
  - Integrated `stb` library into `third_party/`.
  - Migrated `imgui` and `spdlog` to standard Git submodules and updated target linking.
  - Added `glfw3` dependency explicitly to the Raylib backend target for link group resolution.

- **Deprecations**: Removed legacy root `Makefile`, platform shell scripts, and unreferenced `RaylibTexture` implementation files.

## [0.4.1] - 2026-04-09

Subsystem access refactoring, strict compiler shadow warning fixes, and application destructor safety improvements.

### Breaking Changes

- **Subsystem Access API**: Removed `GetContext()` in favor of direct subsystem accessors: `GetWindow()`, `GetInput()`, and `GetRenderer()`.
- **Backend Directory Structure**: Reorganized internal backend header files under `include/Backend/`.
- **Window Dimension Types**: Standardized all window dimensions and screen coordinate parameters from `uint32_t` to signed `int` across backend implementations and user code.

### Added

- **Direct Subsystem Accessors**: Added `GetWindow()`, `GetInput()`, and `GetRenderer()` convenience methods across core classes.

### Changed

- **Window API Naming**: Renamed window sizing and title setter methods across backend implementations and demo examples.
- **Application Clean Up**: Removed redundant `GetWindow()` calls in `Application` to streamline subsystem delegation.

### Fixed

- **Application Lifecycle**: Corrected `Application` destruction sequence to guarantee proper backend teardown and shutdown ordering.
- **Shadowing Warnings**: Renamed constructor parameters in `Vector2`, `Vector3`, and `Color` to resolve `-Wshadow` and `-Werror` warnings on GCC/Clang.
- **Assertion Macro Fix**: Removed invalid redundant second parameter from the `RS_CORE_ASSERT_TRUE` scene assertion macro.
- **Header Order & Windows Compatibility**: Guaranteed `pch.hpp` is included first in `BackendFactory.cpp` and added `#ifndef NOMINMAX` guards around global `NOMINMAX` definitions.

## [0.4.0] - 2026-04-08

Major milestone release introducing a stack-based Scene Architecture, complete subsystem interface renaming, `vcpkg` dependency integration, and enhanced logging.

### Breaking Changes

- **Scene System**: Introduced the `Scene` and `SceneManager` architecture, providing stack-based scene management, a complete scene lifecycle, and access to engine subsystems through `EngineContext`.

  - Added lifecycle callbacks: `OnAttach`, `OnStart`, `OnUpdate`, `OnFixedUpdate`, `OnDraw`, `OnDetach`, `OnPause`, and `OnResume`.
  - Each scene receives an `EngineContext` to access the engine's core subsystems.
  - `SceneManager` manages a LIFO stack of scenes with push/pop/replace operations.

- **Renderer API Refactor**: Replaced `IRenderer` interface with `RendererAPI` and removed VSync handling from rendering interface.
- **Window & Input Naming**: Renamed core interfaces from `Window` to `MainWindow` and `Input` to `InputSystem` to prevent compiler symbol collisions.
- **Logging Subsystem**: Replaced the legacy `Logger` class with the unified `Log` class.
- **`vcpkg` Migration**: Replaced internal `vendor/CMakeLists.txt` with manifest-based `vcpkg.json` dependency management.

### Added

- **Window State API**: Added `IsMinimized()` method to `MainWindow` interface and `RaylibWindow` backend implementation.
- **Stream Output Operators**: Added `std::ostream` `operator<<` overloads for `Vector2` and `Vector3`.
- **Localization & Docs**: Added Spanish documentation (`README.es.md`), updated Doxygen config, and added example preview images.

### Changed

- **Application Integration**: Refactored application lifecycle to natively delegate updates and rendering to `SceneManager`.
- **Context Access**: Updated all examples and user code to access subsystems through `GetContext()`.
- **Sanitizer Propagation**: Updated `target_compile_options` and `target_link_options` for sanitizers to `PUBLIC` so flags propagate to consumer targets.

### Fixed

- Corrected pre-commit hook exclusions for trailing whitespace and mixed line endings.
- Updated `markdownlint` and `codespell` configurations to ignore third-party dependencies and build artifact directories.

### Build & CI Infrastructure

- **Package Management**: Integrated `vcpkg` manifest mode via `CMAKE_TOOLCHAIN_FILE` and updated build presets.
- **CI Workflows**: Streamlined Windows/Linux GitHub Actions build steps, refined dependency caching, and optimized CodeQL analysis execution.
- **Environment Scripts**: Improved cross-platform environment setup scripts (`setup.ps1` / `setup.sh`).

## [0.3.1] - 2026-03-19

Overhaul of procedural generation and math utilities, featuring automatic random seeding, corrected noise hashing, and backend safety guards.

### Added

- **`NoiseLandscape` Example**: Added a procedural terrain showcase demonstrating Perlin, Simplex, Cellular, Value, and FBM noise modes with sky parallax and controls.
- **Runtime Seeding API**: Added `SeedRandom()` and `GetNoiseSeed()` to the `Math::Random` namespace.

### Changed

- **Automatic Seeding**: Random seeds are now automatically generated on startup via `std::random_device`.
- **Noise Hashing**: Updated noise hashing functions to incorporate the active runtime seed.
- **FBM Terminology**: Renamed the `scale` parameter in FBM functions to `lacunarity`.

#### Examples

- **`BouncingBalls`**: Replaced C-style casts and migrated to window-relative positioning.
- **`LissajousCurves`**: Moved phase animation logic into `FixedUpdate()` and removed the manual accumulator.
- **`Mouse2D`**: Separated input and update logic from `Draw()` into `Update()`.

### Fixed

- **`RaylibRenderer` & `RaylibWindow`**: Fixed `ClearScreen` color conversion by scaling normalized `[0.0, 1.0]` floats to 8-bit `[0, 255]` bytes instead of truncating, and zero-initialized member variables in `RaylibWindow`.
- **Vector Division**: Improved division operators in `Vector2` and `Vector3` to safely handle `NaN` denominators.
- **`CellularNoise`**: Resolved diagonal artifacts caused by overlapping feature-point hashes.
- **`SimplexNoise`**: Reimplemented proper triangle-lattice skew/unskew transformation matrices.
- **Main Loop Warnings**: Resolved signed/unsigned integer comparison warnings.

## [0.3.0] - 2026-02-14

Major architecture rewrite transitioning **Raysim** from a monolithic `canvas` model to an event-driven application lifecycle and abstract rendering interface.

### Breaking Changes

- **Application Architecture**: Replaced the monolithic `Canvas` class with the `Application` base class and explicit lifecycle methods: `Setup()`, `Update(dt)`, `FixedUpdate(fixedDt)`, and `Draw(alpha)`.
- **Input System Overhaul**: Replaced `InputManager` with the new `IInput` abstract interface for keyboard, mouse, and scroll events.
- **Color API Simplification**: Simplified `Color` into a plain RGBA struct; replaced factory methods in favor of the `Colors` namespace with `constexpr` presets.

### Added

#### Architecture & Subsystems

- **Backend Abstraction**: Added `IRenderer`, `IWindow`, and `IInput` interfaces with a Raylib backend implementation.
- **`BackendFactory`**: Added backend creation and dependency to decouple user code from any specific graphics library.
- **`ApplicationConfig`**: Added declarative application configuration.
- **Time System**: Added frame timing, fixed timestep accumulation, time scaling, pause/resume, and FPS tracking.
- **Assertions Framework**: Added the debug-only assertion system controlled via the `RS_ENABLE_ASSERTS` macro definition.

#### Math

- **Math Utilities**: Extended `Math.hpp` with `Clamp`, `Lerp`, `InverseLerp`, `Remap`, `PingPong`, `Repeat`, and trigonometric constants.
- **Vector Improvements**: Extended `Vector2` and `Vector3` with `constexpr` operations and geometric utilities including `Limit`, `SetMag`, `Project`, `Reflect`, and `Rotate`.

#### Graphics

- **Color System**: Simplified `Color` into a plain RGBA type with interpolation and HSV conversion (`FromHSV`) support.
- **Colors Namespace**: Added `constexpr` predefined colors.
- **OriginMode**: Extracted `OriginMode` into a dedicated header.

### Changed

- **Examples Restructuring**: Standardized example names to PascalCase (`BouncingBalls`, `LissajousCurves`, `Mouse2D`), isolated CMakeLists per example, and consolidated shared assets under `examples/shared/assets/`.
- **Graphics Abstraction**: Updated `Shapes` and `Texts` to draw via the new `IRenderer` abstraction.
- **Logging Engine**: Reworked logger infrastructure with initialization checks and inline `Flush()`.

### Build

- **Modular CMake**: Restructured build logic into modular configuration files (`CompilerFlags.cmake`, `GlobalSettings.cmake`, `ProjectOptions.cmake`, `Sanitizers.cmake`).
- **Dependency Resolution**: Added system-first `spdlog` resolution using `find_package()` with a `FetchContent` fallback.
- **CI Workflows**: Added `libspdlog-dev` system package support to CI build runners and removed headless mouse detection testing.
- **Tooling**: Rewrote Makefile for improved consistency and usability, updating minimum CMake requirement to 3.19.

## [0.2.4] - 2026-01-20

### Changed

- **Asset Resolution**: Moved the `resources` directory under `examples/` and updated `FontManager` to resolve fonts relative to the current executable directory.
- **Repository Hygiene**: Improved cleanup scripts and updated `.gitignore` to exclude build artifacts and temporary binaries.

## [0.2.3] - 2026-01-10

### Added

- **Initial Core Subsystems**: Introduced the foundational Raysim architecture, including `Canvas`, `FontManager`, `InputManager`, `TimeManager`, and `spdlog` integration.
- **Drawing & Text**: Added primitive rendering through `Shapes` and text rendering with alignment and origin controls.
- **Math & Color**: Added the initial `Vector2`, `Vector3`, `Random`, and `Color` utilities.
- **Asset Bundling**: Integrated Open Sans font assets and Raylib dependencies.
- **Examples**: Added the initial `BouncingBalls`, `LissajousCurves`, and `MouseDetection` demonstrations.
