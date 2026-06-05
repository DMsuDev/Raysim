/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

// ============================================================================
// Core Bootstrap
// ----------------------------------------------------------------------------
// API.hpp transitively brings in PlatformDetection.hpp, which defines
// RS_PLATFORM_WINDOWS / RS_PLATFORM_LINUX before any platform-sensitive
// code below runs.
// ============================================================================
#include "Raysim/Core/API.hpp"

#include <memory>

// ============================================================================
// Debug Utilities
// ----------------------------------------------------------------------------
// RS_DEBUGBREAK() triggers a hardware breakpoint in the attached debugger.
// Outside debug builds it expands to nothing, so call sites are always valid.
//
// RS_ENABLE_ASSERTS is defined only in debug builds; Assert.hpp gates all
// assertion macros on this symbol.
// ============================================================================
#ifdef RS_BUILD_DEBUG
	#ifdef RS_PLATFORM_WINDOWS
		#define RS_DEBUGBREAK() __debugbreak()
	#elif defined(RS_PLATFORM_LINUX)
		#include <signal.h>
        #define RS_DEBUGBREAK() __builtin_trap()
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	// Enable assertions only in debug builds
    #define RS_ENABLE_ASSERTS
#else
	#define RS_DEBUGBREAK()
#endif

// ============================================================================
// Preprocessor Utilities
// ----------------------------------------------------------------------------
// RS_EXPAND_MACRO  -> forces a macro argument to expand before stringification
//                     or token-pasting; required for the VA_ARGS argument-
//                     counting trick under MSVC's non-conformant preprocessor.
//
// RS_STRINGIFY     -> converts an already-expanded token to a string literal,
//                     used to embed expression text in diagnostic messages.
//                     Note: Before called as RS_STRINGIFY_MACRO
//
// BIT(n)           -> produces the value 1 << n as a compile-time constant,
//                     suitable for defining flag enumerations.
//                     Note: for flags beyond bit 30, prefer BIT64 or an enum.
// ============================================================================
#define RS_EXPAND_MACRO(x)  x
#define RS_STRINGIFY(x)     #x

#define BIT(x) (1 << (x))

// ============================================================================
// Smart-Pointer Aliases & Factory Helpers
// ----------------------------------------------------------------------------
// Scope<T>   -> exclusive ownership (std::unique_ptr).  Prefer this by
//               default; zero runtime overhead over a raw pointer.
//
// Shared<T>  -> shared ownership (std::shared_ptr).  Use only when multiple
//               owners genuinely exist; carries reference-count overhead.
//
// CreateScope / CreateShared -> thin wrappers around make_unique / make_shared
//               with perfect forwarding.  [[nodiscard]] prevents silently
//               discarding the returned smart pointer.
// ============================================================================
namespace RS
{
    /// @brief Exclusive-ownership pointer alias (zero overhead over raw pointer).
    /// @tparam T Must be a complete type at the point of destruction.
    template <typename T>
    using Scope = std::unique_ptr<T>;

    /// @brief Shared-ownership pointer alias (reference-counted).
    /// @tparam T Must be a complete type at the point of destruction.
    template <typename T>
    using Shared = std::shared_ptr<T>;

    /// @brief  Constructs a T in exclusive ownership via perfect forwarding.
    /// @tparam T    Type to construct; must be a complete type.
    /// @tparam Args Constructor argument types (deduced).
    /// @return Scope<T> owning the newly created object.
    /// @note   Exception-safe alternative to Scope<T>(new T(...)).
    template <typename T, typename... Args>
    [[nodiscard]] constexpr Scope<T> CreateScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /// @brief  Constructs a T in shared ownership via perfect forwarding.
    /// @tparam T    Type to construct; must be a complete type.
    /// @tparam Args Constructor argument types (deduced).
    /// @return Shared<T> owning the newly created object.
    /// @note   Carries reference-count overhead; prefer CreateScope() unless
    ///         multiple owners genuinely exist.
    template <typename T, typename... Args>
    [[nodiscard]] constexpr Shared<T> CreateShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
} // namespace RS

// ============================================================================
// Framework Subsystems
// ----------------------------------------------------------------------------
// Assert.hpp -> assertion macros (RS_ASSERT / RS_CORE_ASSERT).
//               Internally includes Log.hpp; no need to list it here.
//
// Instrumentor.hpp -> lightweight scope-based profiler; produces a Chrome
//                     Tracing JSON file when RS_PROFILE is defined.
// ============================================================================
#include "Raysim/Core/Assert.hpp"
#include "Raysim/Debug/Instrumentor.hpp"
