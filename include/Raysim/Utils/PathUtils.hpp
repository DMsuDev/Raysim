#pragma once

#include <string>
#include <filesystem>

namespace RS::Utils {

enum class PathBase {
    Executable,   // Directory of the running executable (e.g. "C:\MyApp\bin")
    Working,      // Current working directory (e.g. "C:\Users\Me")
    Assets        // Predefined "assets" directory inside the executable directory (e.g. "C:\MyApp\bin\assets")
};

// --- Base paths ---
std::filesystem::path GetExecutableDirectory();
std::filesystem::path GetWorkingDirectory();
std::filesystem::path GetAssetsDirectory();

// --- Path utils ---
bool IsAbsolute(const std::filesystem::path& path);

std::filesystem::path ResolvePath(
    const std::filesystem::path& path,
    PathBase base = PathBase::Assets
);

// --- Convenience ---
std::string ResolvePathStr(
    const std::string& path,
    PathBase base = PathBase::Assets
);

} // namespace RS::Utils
