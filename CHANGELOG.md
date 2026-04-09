# Changelog

All notable changes to this project will be documented in this file.

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

## [0.4.0] - 2026-04-08

### Added

- **Scene System**: New `Scene` base class and `SceneManager` for scene-based architecture
  - `Scene` provides lifecycle callbacks: `OnAttach`, `OnStart`, `OnUpdate`, `OnFixedUpdate`, `OnDraw`, `OnDetach`, `OnPause`, `OnResume`
  - `SceneManager` manages a LIFO stack of scenes with push/pop/replace operations
  - Each scene receives an `EngineContext` for subsystem access (Window, Renderer, Input)
- New `IsMinimized()` method to `Window` interface and `RaylibWindow` implementation

### Changed

- **Application Refactor**: Applications now integrate with `SceneManager` for scene management
- **Window/Input Access**: Examples updated to use `GetContext()` instead of direct `Window`/`Input` member access
- Refactored window system and application flow for better scene integration
- Replaced `IRenderer` interface with `RendererAPI` and updated backend implementations
- Updated input handling to use new `Input` interface and related classes
- Added stream output operators (`<<`) for `Vector2` and `Vector3`
- Replaced `Logger` with `Log` class for consistent logging across the application

### Fixed

- CMake: sanitizer flags now correctly propagate to targets (`PUBLIC` instead of `PRIVATE`)
- `codespell` skip pattern corrected for CMakeFiles

### Build

- Migrated dependencies to vcpkg (added `vcpkg.json`, removed vendor CMakeLists.txt)
- Updated CMake minimum to 3.28
- Environment setup scripts enhanced for cross-platform compatibility

## [0.3.1] - 2026-03-19

### Added

- New demo `NoiseLandscape` showcasing layered noise-based terrain with dynamic reseeding and a sky gradient

### Fixed

- `ClearScreen(Vector3)` now correctly maps normalised [0,1] floats to [0,255] bytes (was truncating without scaling)
- Backend factory null-pointer crash: `Application` validates that Renderer, Window, and Input were created before use
- Uninitialized members in `RaylibWindow` (`width_`, `height_`, `prevWidth_`, `prevHeight_` now default to 0)
- Signed/unsigned mismatch warning in the main loop (`stepsTaken` vs `maxFixedSteps`)
- `Vector2` and `Vector3` division operators now reject NaN in addition to zero
- Diagonal artifact in `CellularNoise` (X and Y feature-point hashes were identical)
- `SimplexNoise` rewritten to use proper triangle-lattice with skew/unskew

### Changed

- **Math::Random** - Seed is fully random on startup via `std::random_device`; noise functions incorporate the seed into all hashes
- Added `SeedRandom()` and `GetNoiseSeed()` to the Random API
- FBM parameter renamed from `scale` to `lacunarity`
- NoiseLandscape demo: terrain layers support `Reseed()`, sky uses `Color::Lerp` gradient, removed dead code
- BouncingBalls demo: replaced C-style casts, uses `Math::Random::Choice()`, window-relative UI positions
- LissajousCurves demo: window-relative layout, phase animation moved to `FixedUpdate`, removed manual accumulator
- Mouse2D demo: input/logic moved from `Draw` to `Update`, fixed typo `mouseSmootedPos`, window-relative positions

## [0.3.0]

Major architecture rewrite. The monolithic `Canvas` class has been replaced with a proper `Application` base class and a clean backend abstraction layer.

### Architecture

- Replaced `Canvas` with `Application` base class and lifecycle methods: `Setup()`, `Update(dt)`, `FixedUpdate(fixedDt)`, `Draw(alpha)`
- Introduced abstract interfaces (`IRenderer`, `IWindow`, `IInput`) with a Raylib backend implementation
- Added `BackendFactory` to decouple user code from any specific graphics library
- Added `ApplicationConfig` struct for declarative app configuration

### Core

- New `Time` system: frame timing, fixed timestep accumulator, time scale, pause/resume, smoothed FPS tracking
- New `Assert` module with debug-only runtime assertions (`RS_ASSERT`)
- Logger rewritten with initialization checks and inline `Flush()`
- Replaced `InputManager` with `IInput` interface (keyboard, mouse, scroll)

### Math

- `Vector2` and `Vector3` enhanced with `constexpr` operators, geometric utilities (`Limit`, `SetMag`, `Project`, `Reflect`, `Rotate`), and improved documentation
- New `Math.hpp` utilities: `Clamp`, `Lerp`, `Remap`, `PingPong`, `Repeat`, `InverseLerp`, trig constants

### Graphics

- `Color` simplified to a plain RGBA struct with `Lerp`, `FromHSV`, and operator overloads
- `Colors` namespace with `constexpr` presets (replaces old factory methods)
- `OriginMode` enum extracted to its own header
- `Shapes` and `Texts` updated to use the new rendering interfaces

### Examples

- Renamed to PascalCase: `BouncingBalls`, `LissajousCurves`, `Mouse2D`
- Shared assets moved to `examples/shared/assets/`
- Each example has its own `CMakeLists.txt`

### Build

- CMake restructured into modular files (`CompilerFlags.cmake`, `GlobalSettings.cmake`, `ProjectOptions.cmake`, `Sanitizers.cmake`)
- spdlog resolved via `find_package` with `FetchContent` fallback
- Makefile rewritten for consistency and usability
- Minimum CMake version updated to 3.19
