#pragma once
#include <string>

namespace RS
{
    /**
     * @struct WindowProps
     * @brief Configuration properties for creating a window.
     *
     * Contains the title, width, and height of the window. This struct is used
     * to pass initialization parameters to the Window constructor, allowing for flexible
     * window creation with default values.
     */
    struct WindowProps
    {
        std::string Title = "Raysim App"; // The title of the window.
        int Width = 1600;                 // The width of the window in pixels.
        int Height = 900;                 // The height of the window in pixels.

        WindowProps() = default;
        WindowProps(const std::string &title, int width, int height)
            : Title(title), Width(width), Height(height) {}
    };
} // namespace RS
