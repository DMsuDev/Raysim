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

/**
 * @file ImGuiStyles.hpp
 * @brief Style definitions and utilities for ImGui UI theming.
 *
 * This file defines a collection of functions to apply custom visual styles
 * to the ImGui user interface. It includes predefined themes such as "Dark",
 * "Light", and "Solarized", as well as an API to register and apply custom
 * styles by name. This allows developers to easily adapt the appearance of
 * their UI to their preferences or thematic needs.
 *
 * The style system is extensible, allowing new themes to be added without modifying
 * the base code, simply by registering new style functions with a unique name.
 */

#include "imgui.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace RS::ImGuiStyles {

using StyleFunc = void(*)();

// Built-in style identifiers (convenience enum for client code)
enum class BuiltinStyle
{
    Classic,
    Dark,
    Light,
    Solarized
};

// ===========================================================================
// Style definitions
// ===========================================================================

static inline void SetClassicTheme()
{
    ImGui::StyleColorsClassic();
}

static inline void SetDarkTheme()
{
    ImGui::StyleColorsDark();

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

static inline void SetLightTheme()
{
    ImGui::StyleColorsLight();

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.94f, 0.94f, 0.94f, 1.0f };
}

static inline void SetSolarizedTheme()
{
    ImGui::StyleColorsDark();
    auto& c = ImGui::GetStyle().Colors;
    const ImVec4 base03  = ImVec4{ 0.0f/255.0f, 43.0f/255.0f, 54.0f/255.0f, 1.0f };
    const ImVec4 base02  = ImVec4{ 7.0f/255.0f, 54.0f/255.0f, 66.0f/255.0f, 1.0f };
    const ImVec4 base0   = ImVec4{ 131.0f/255.0f, 148.0f/255.0f, 150.0f/255.0f, 1.0f };
    const ImVec4 blue    = ImVec4{ 38.0f/255.0f, 139.0f/255.0f, 210.0f/255.0f, 1.0f };

    c[ImGuiCol_WindowBg] = base03;
    c[ImGuiCol_Header] = base02;
    c[ImGuiCol_HeaderHovered] = base0;
    c[ImGuiCol_Button] = base02;
    c[ImGuiCol_ButtonHovered] = blue;
}

// ===========================================================================
// Style registry and utilities
// ===========================================================================

static inline std::unordered_map<std::string, StyleFunc>& Registry()
{
    static std::unordered_map<std::string, StyleFunc> registry = {
        { "Classic", SetClassicTheme },
        { "Dark", SetDarkTheme },
        { "Light", SetLightTheme },
        { "Solarized", SetSolarizedTheme }
    };
    return registry;
}

/**
 * @brief Applies a style by name.
 *
 * Searches the style registry for the given name and, if found, executes the
 * associated function to apply the style. Returns true if the style was found
 * and applied, or false if no style with the given name was found.
 *
 * @param name The name of the style to apply (e.g. "Dark", "Light").
 * @return true if the style was successfully applied, false if not found.
 */
static inline bool ApplyStyle(const std::string& name)
{
    auto& reg = Registry();
    auto it = reg.find(name);
    if (it == reg.end()) return false;
    it->second();
    return true;
}

/**
 * @brief Applies a built-in style by enum identifier.
 *
 * This is a convenience overload that allows client code to apply a built-in
 * style using the `BuiltinStyle` enum instead of a string name. It simply
 * maps the enum value to the corresponding style name and calls the string
 * version of `ApplyStyle`.
 *
 * @param s The built-in style to apply (e.g. BuiltinStyle::Dark).
 * @return true if the style was successfully applied, false if not found.
 */
static inline bool ApplyStyle(BuiltinStyle s)
{
    switch (s)
    {
        case BuiltinStyle::Classic: return ApplyStyle("Classic");
        case BuiltinStyle::Dark: return ApplyStyle("Dark");
        case BuiltinStyle::Light: return ApplyStyle("Light");
        case BuiltinStyle::Solarized: return ApplyStyle("Solarized");
        default: return false;
    }
}

/**
 * @brief Registers a new style function with a unique name.
 *
 * This allows client code to add custom styles to the registry at runtime. The
 * provided function will be associated with the given name, and can later be
 * applied using `ApplyStyle` with that name. If a style with the same name
 * already exists, it will be overwritten.
 *
 * @param name The unique name for the style (e.g. "MyCustomStyle").
 * @param f The function that applies the style when called.
 */
static inline void RegisterStyle(const std::string& name, StyleFunc f)
{
    Registry()[name] = f;
}

/**
 * @brief Lists the names of all available styles in the registry.
 *
 * This utility function returns a vector of strings containing the names of
 * all styles currently registered in the system. This can be used to populate
 * UI elements (e.g. dropdowns) that allow users to select from available styles.
 *
 * @return A vector of style names currently registered.
 */
static inline std::vector<std::string> ListAvailableStyles()
{
    std::vector<std::string> out;
    out.reserve(Registry().size());
    for (auto& p : Registry()) out.push_back(p.first);
    return out;
}

} // namespace ImApp::ImGuiStyles
