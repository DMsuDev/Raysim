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

#include <string>
#include <filesystem>

namespace RS::Utils {

enum class PathBase {
    Executable,   // Directory of the running executable (e.g. "C:\MyApp\bin")
    Working,      // Current working directory (e.g. "C:\Users\Me")
    Assets        // Predefined "assets" directory inside the executable directory (e.g. "C:\MyApp\bin\assets")
};

// Base paths
std::filesystem::path GetExecutableDirectory();
std::filesystem::path GetWorkingDirectory();
std::filesystem::path GetAssetsDirectory();

// Path utils
bool IsAbsolute(const std::filesystem::path& path);

std::filesystem::path ResolvePath(
    const std::filesystem::path& path,
    PathBase base = PathBase::Assets
);

// Convenience
std::string ResolvePathStr(
    const std::string& path,
    PathBase base = PathBase::Assets
);

} // namespace RS::Utils
