# Changelog

All notable changes to this project will be documented in this file.

---

## [0.93.0] - 2026-05-12

### Added

- `EventBus`: type-safe publish/subscribe event bus owned by the `Application` and exposed through `EngineContext::Bus`.
  Unlike the layer-based propagation (blocking, ordered), the `EventBus` is a broadcast channel: all active subscribers receive the event in subscription order regardless of the `Handled` flag. Typical use cases include audio systems reacting to focus events, debug overlays subscribing to hotkeys, and achievement systems listening for game events.

  ```cpp
  // Subscribe in OnAttach
  m_SubID = GetContext().Bus->Subscribe<KeyPressedEvent>([](const KeyPressedEvent& e) {
      RS_LOG_INFO("Key pressed: {}", static_cast<int>(e.GetKeyCode()));
  });

  // Unsubscribe in OnDetach
  GetContext().Bus->Unsubscribe(m_SubID);
  ```

- `EventDrivenInput`: abstract intermediate `Input` implementation for event-driven backends (GLFW, SDL2, Win32 raw input, etc.). Maintains internal per-frame state arrays (Down/Pressed/Released/Repeating) for keyboard and mouse that are populated through `OnEvent()`. Backends that receive input via OS callbacks inherit from `EventDrivenInput` instead of from `Input` directly; polling backends (e.g. Raylib) continue to inherit from `Input` as before.

- `GamepadCodes`: backend-agnostic gamepad button and axis definitions (`GamepadButton`, `GamepadAxis`) using an Xbox-style layout. Maps face buttons (A/B/X/Y), shoulder/trigger buttons (LB/RB/LT/RT), thumb-sticks, bumpers, and the D-Pad. Backends translate their native constants to this enum through a dedicated translation layer.

- `Angle` type (`include/Raysim/Math/Types/Angle.hpp`): strongly-typed wrapper for angles stored internally in radians. Prevents accidental mixing of radians and degrees. All construction must go through the factory methods `Angle::Radians()` or `Angle::Degrees()`. Supports full arithmetic and comparison operators, wrapping utilities (`WrapPositive`, `WrapSigned`), and user-defined literals via `RS::Math::Literals`:

  ```cpp
  using namespace RS::Math::Literals;

  Angle a = 90.0_deg;
  Angle b = Angle::Radians(1.5707f);

  float r = a.AsRadians();     // ~1.5707
  float d = b.AsDegrees();     // ~90.0
  Angle c = a + b;             // ~180°
  Angle w = c.WrapSigned();    // wrapped to (-180°, 180°]
  ```

- `Vec2f` / `Vec2i` type aliases: `Math::Vec2f` (`Vector2<float>`) and `Math::Vec2i` (`Vector2<int>`) replace the previous untyped `Math::Vec2` usage throughout the codebase and public API. All built-in examples have been updated accordingly.

- `Vector2::RotatedBy(Angle)`: rotates a 2D vector around the origin by a strongly-typed `Angle` value, eliminating ambiguity between radians and degrees at call sites.

- MathUtils additions (`include/Raysim/Math/Utils/MathUtils.hpp`):
  - `SmoothDamp`: smoothly interpolates a value towards a target with a configurable smooth time and optional max speed.
  - `MoveTowards`: moves a value towards a target without overshooting.
  - `EpsilonEquals`: floating-point approximate equality using a configurable epsilon.
  - `Saturate`: clamps a value to `[0, 1]`.
  - `Fract`: returns the fractional part of a floating-point value.

### Changed

- `EngineContext` now exposes `EventBus* Bus`; scenes and layers can access the bus through `GetContext().Bus` without touching the `Application` class directly.
- `Input` interface extended with gamepad query methods (`IsGamepadButtonDown`, `IsGamepadButtonPressed`, `GetGamepadAxis`, `IsGamepadConnected`, etc.) using the new `GamepadButton` and `GamepadAxis` enums.
- `Vector2` template now includes `Angle.hpp` so that `RotatedBy(Angle)` is available without additional includes.
- All example projects (`BouncingBalls`, `LissajousCurves`, `Mouse2D`, `NoiseLandscape`, `SceneShowcase`) updated to use `Math::Vec2f` / `Math::Vec2i` aliases instead of the old `Math::Vec2` usage.
- `imgui` and `spdlog` were removed as submodules and are now integrated locally in `third_party/` with CMake targets defined in `cmake/ThirdParty/`.

---

## [0.91.1] - 2026-05-08

### Fixed

- `$<LINK_GROUP:RESCAN>` generator expression replaced with a plain `raylib glfw` link on MSVC. The GNU-style `--start-group`/`--end-group` linker feature is not supported by the MSVC linker, causing configure errors on Visual Studio generators.
- MSVC warning D9025 (`/W1` overridden by `/w`) in third-party targets. `rs_third_party_setup()` now passes `/W0` instead of `/w` to correctly suppress the warning level.
- MSVC warning C4996 (`fopen` deprecation) in `STBTrueTypeProvider.cpp`. `_CRT_SECURE_NO_WARNINGS` is now defined locally for that translation unit only (stb is third-party code).
- MSVC warning C4100 (unreferenced parameter `window`) in `RaylibImGuiBackend::Init`.

### Changed

- `glfw3` added back as an optional vcpkg dependency for the `raylib` backend; `cmake/Backends/raylib.cmake` links `glfw` alongside `raylib` when found (improves compatibility on some configurations).
- Sanitizer CMake module (`Sanitizers.cmake`) extended with improved MSVC support.

---

## [0.91.0] - 2026-05-07

### Added

- `SceneShowcase` example: multi-scene demo (StarField, PlasmaArt, ClockMandala) navigable via an `ImGui` overlay (`SceneNavLayer` layer).

### Changed

- `ApplicationConfig` is now the standard entry point for window configuration; `config.Window.Title/Width/Height` replaces per-call `GetWindow().SetTitle/SetSize`.
- Backend system refactored: `RS_BACKEND` CMake cache option (`raylib` [default] / `glfw_opengl` / `sfml` / `sdl2`) replaces the old `RS_IMGUI_BACKEND` approach. `BackendRegistry.cmake` dynamically loads `cmake/Backends/<name>.cmake` for the selected backend, and `src/CMakeLists.txt` now links against the unified `rs_backend` meta-target instead of hardcoded per-library entries.
- `vcpkg.json`: removed `glfw3` dependency (not needed for the Raylib backend; restore it when switching to `glfw_opengl`).
- README files (English and Spanish) updated: added `SceneShowcase` to demos table, added Setup section, added ImGui Integration module section, removed unimplemented `Texture` entry from Graphics table, replaced `SetInitialScene` with `ChangeScene` throughout, updated Quick Start snippet to use `ApplicationConfig` and `Math::Vec2`.

### Removed

- `SetInitialScene<T>()` removed. Use `ChangeScene<T>()` after `RegisterScene<T>()` to activate the first scene.

---

## [0.6.1] - 2026-04-25

### Added

- `RS_SCENE` macro for automatic scene type registration and name/ID generation.
- `Vector2Int` and `Vector3Int` structs for 2D and 3D integer vector operations.
- Logging level management with registries for unique and TTL-based logging.
- `NVI` (Non-Virtual Interface) pattern added to `Window` for better encapsulation and future-proofing.
- `GetWindow()`, `GetInput()`, and `GetRenderer()` moved directly onto `Scene` for easier access without needing `GetContext()`.
- Memory management and assertion utilities consolidated into `Base.hpp`.
- `EngineContext` struct introduced for passing subsystem references to scenes.

### Changed

- Replaced `AddScene` with `RegisterScene` and `SetInitialScene` for improved scene management.
- Streamlined `Scene` and `SceneManager` classes, removing redundant methods.
- `Window->OnUpdate` replaced by `PollEvents` and `SwapBuffer` for clearer separation.
- RaylibWindow methods updated to follow the new `Window` interface naming conventions.
- Compiler settings reorganized into separate files: `CompilerOptimization.cmake`, `CompilerOptions.cmake`, `CompilerWarnings.cmake`.
- Win32 definition exclusions reorganized in header files.
- spdlog linking simplified with FetchContent fallback.
- Library alias creation simplified.
- `pch.hpp` include paths updated to use direct references.
- Operator formatting consistency improved in `Vector2Int` and `Vector3Int`.
- Removed `Scene.cpp` from source list.

### Fixed

- `pch.hpp` now included first in all source files to ensure precompiled header usage.
- `NOMINMAX` definition now properly guarded to prevent conflicts with other headers.
- `RS_CORE_ASSERT_TRUE` macro in scene module fixed to remove unused second parameter.

### Build

- Modernized CMake configuration with modular structure.
- Added `rs_options` for configurable build settings.
- Integrated LTO (Link Time Optimization) configuration.
- Added sanitizer support configuration.
- Fixed shared library linking issues.
- Added `imgui` and `spdlog` as third-party submodules.
- Added GitHub workflow to automate vcpkg baseline updates.

---

## [0.4.1] - 2026-04-09

### Added

- Direct access methods to core subsystems, replacing `GetContext()` usage.

### Changed

- Removed redundant `Application::GetWindow()` method.
- Moved headers from `include/Raysim/backend/` to `include/backend/`.
- Renamed window size and title methods across examples and backend.
- Updated `Window().GetSize()` return types from `uint32_t` to `int`.
- Improved constructor parameter naming on `Vector2`, `Vector3` and `Color` to resolve `-Werror` and `-Wshadow` warnings.

### Fixed

- Ensured `pch.hpp` is included first in `BackendFactory.cpp`.
- Guarded `NOMINMAX` definition in `pch.hpp`.
- Removed unused second parameter from `RS_CORE_ASSERT_TRUE` in scene module.
- Fixed `Application` destructor behavior.

---

## [0.4.0] - 2026-04-08

### Added

- Scene System: new `Scene` base class and `SceneManager` for scene-based architecture.
  - `Scene` provides lifecycle callbacks: `OnAttach`, `OnStart`, `OnUpdate`, `OnFixedUpdate`, `OnDraw`, `OnDetach`, `OnPause`, `OnResume`.
  - `SceneManager` manages a LIFO stack of scenes with push/pop/replace operations.
  - Each scene receives an `EngineContext` for subsystem access (Window, Renderer, Input).
- New `IsMinimized()` method on `Window` interface and `RaylibWindow` implementation.

### Changed

- Application Refactor: applications now integrate with `SceneManager` for scene management.
- Window/Input Access: examples updated to use `GetContext()` instead of direct `Window`/`Input` member access.
- Refactored window system and application flow for better scene integration.
- Replaced `IRenderer` interface with `RendererAPI` and updated backend implementations.
- Updated input handling to use new `Input` interface and related classes.
- Added stream output operators (`<<`) for `Vector2` and `Vector3`.
- Replaced `Logger` with `Log` class for consistent logging across the application.

### Fixed

- CMake: sanitizer flags now correctly propagate to targets (`PUBLIC` instead of `PRIVATE`).
- `codespell` skip pattern corrected for CMakeFiles.

### Build

- Migrated dependencies to vcpkg (added `vcpkg.json`, removed vendor CMakeLists.txt).
- Updated CMake minimum to 3.28.
- Environment setup scripts enhanced for cross-platform compatibility.

---

## [0.3.1] - 2026-03-19

### Added

- New demo `NoiseLandscape` showcasing layered noise-based terrain with dynamic reseeding and a sky gradient.

### Fixed

- `ClearScreen(Vector3)` now correctly maps normalised [0,1] floats to [0,255] bytes (was truncating without scaling).
- Backend factory null-pointer crash: `Application` validates that Renderer, Window, and Input were created before use.
- Uninitialized members in `RaylibWindow` (`width_`, `height_`, `prevWidth_`, `prevHeight_` now default to 0).
- Signed/unsigned mismatch warning in the main loop (`stepsTaken` vs `maxFixedSteps`).
- `Vector2` and `Vector3` division operators now reject NaN in addition to zero.
- Diagonal artifact in `CellularNoise` (X and Y feature-point hashes were identical).
- `SimplexNoise` rewritten to use proper triangle-lattice with skew/unskew.

### Changed

- Math::Random: seed is fully random on startup via `std::random_device`; noise functions incorporate the seed into all hashes.
- Added `SeedRandom()` and `GetNoiseSeed()` to the Random API.
- FBM parameter renamed from `scale` to `lacunarity`.
- NoiseLandscape demo: terrain layers support `Reseed()`, sky uses `Color::Lerp` gradient, removed dead code.
- BouncingBalls demo: replaced C-style casts, uses `Math::Random::Choice()`, window-relative UI positions.
- LissajousCurves demo: window-relative layout, phase animation moved to `FixedUpdate`, removed manual accumulator.
- Mouse2D demo: input/logic moved from `Draw` to `Update`, fixed typo `mouseSmootedPos`, window-relative positions.

---

## [0.3.0]

Major architecture rewrite. The monolithic `Canvas` class has been replaced with a proper `Application` base class and a clean backend abstraction layer.

### Architecture

- Replaced `Canvas` with `Application` base class and lifecycle methods: `Setup()`, `Update(dt)`, `FixedUpdate(fixedDt)`, `Draw(alpha)`.
- Introduced abstract interfaces (`IRenderer`, `IWindow`, `IInput`) with a Raylib backend implementation.
- Added `BackendFactory` to decouple user code from any specific graphics library.
- Added `ApplicationConfig` struct for declarative app configuration.

### Core

- New `Time` system: frame timing, fixed timestep accumulator, time scale, pause/resume, smoothed FPS tracking.
- New `Assert` module with debug-only runtime assertions (`RS_ASSERT`).
- Logger rewritten with initialization checks and inline `Flush()`.
- Replaced `InputManager` with `IInput` interface (keyboard, mouse, scroll).

### Math

- `Vector2` and `Vector3` enhanced with `constexpr` operators, geometric utilities (`Limit`, `SetMag`, `Project`, `Reflect`, `Rotate`), and improved documentation.
- New `Math.hpp` utilities: `Clamp`, `Lerp`, `Remap`, `PingPong`, `Repeat`, `InverseLerp`, trig constants.

### Graphics

- `Color` simplified to a plain RGBA struct with `Lerp`, `FromHSV`, and operator overloads.
- `Colors` namespace with `constexpr` presets (replaces old factory methods).
- `OriginMode` enum extracted to its own header.
- `Shapes` and `Texts` updated to use the new rendering interfaces.

### Examples

- Renamed to PascalCase: `BouncingBalls`, `LissajousCurves`, `Mouse2D`.
- Shared assets moved to `examples/shared/assets/`.
- Each example has its own `CMakeLists.txt`.

### Build

- CMake restructured into modular files (`CompilerFlags.cmake`, `GlobalSettings.cmake`, `ProjectOptions.cmake`, `Sanitizers.cmake`).
- spdlog resolved via `find_package` with `FetchContent` fallback.
- Makefile rewritten for consistency and usability.
- Minimum CMake version updated to 3.19.
