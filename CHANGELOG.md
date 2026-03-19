# Changelog

All notable changes to this project will be documented in this file.

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
