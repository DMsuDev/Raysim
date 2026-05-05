#include "pch.hpp"
#include "Raysim/Utils/PathUtils.hpp"

#include <stdexcept>

#if defined(RS_PLATFORM_WINDOWS)
    #include <windows.h>
#elif defined(RS_PLATFORM_LINUX)
    #include <unistd.h>
    #include <limits.h>
#elif defined(RS_PLATFORM_MAC)
    #include <mach-o/dyld.h>
    #include <limits.h>
#endif

namespace RS::Utils {

// ============================================================
// Executable Directory (platform-specific)
// ============================================================

std::filesystem::path GetExecutableDirectory()
{
#if defined(RS_PLATFORM_WINDOWS)

    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    if (len == 0 || len == MAX_PATH)
        throw std::runtime_error("Failed to get executable path");

    return std::filesystem::path(buffer).parent_path();

#elif defined(RS_PLATFORM_LINUX)

    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len <= 0)
        throw std::runtime_error("Failed to read /proc/self/exe");

    buffer[len] = '\0';
    return std::filesystem::path(buffer).parent_path();

#elif defined(RS_PLATFORM_MAC)

    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);

    std::string temp(size, '\0');

    if (_NSGetExecutablePath(temp.data(), &size) != 0)
        throw std::runtime_error("Failed to get executable path");

    return std::filesystem::path(temp).parent_path();

#else
    #error Unsupported platform
#endif
}

// ============================================================
// Other base paths
// ============================================================

std::filesystem::path GetWorkingDirectory()
{
    return std::filesystem::current_path();
}

std::filesystem::path GetAssetsDirectory()
{
    return GetExecutableDirectory() / "assets";
}

// ============================================================
// Helpers
// ============================================================

bool IsAbsolute(const std::filesystem::path& path)
{
    return path.is_absolute();
}

// ============================================================
// ResolvePath
// ============================================================

std::filesystem::path ResolvePath(
    const std::filesystem::path& path,
    PathBase base)
{
    if (path.empty())
    {
        RS_CORE_WARN("ResolvePath: empty path provided");
        return {};
    }

    if (path.is_absolute())
        return path;

    std::filesystem::path basePath;

    switch (base)
    {
        case PathBase::Executable:
            basePath = GetExecutableDirectory();
            break;

        case PathBase::Working:
            basePath = GetWorkingDirectory();
            break;

        case PathBase::Assets:
            basePath = GetAssetsDirectory();
            break;
    }

    return std::filesystem::weakly_canonical(basePath / path);
}

// ============================================================
// String wrapper
// ============================================================

std::string ResolvePathStr(
    const std::string& path,
    PathBase base)
{
    return ResolvePath(std::filesystem::path(path), base).string();
}

} // namespace RS::Utils
