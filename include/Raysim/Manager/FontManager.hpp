#pragma once

#include <string>

namespace RS {

// Opaque handle for raylib Font without exposing raylib headers
struct FontHandle {
    void* data = nullptr;

    FontHandle() = default;
    explicit FontHandle(void* ptr) : data(ptr) {}

    bool IsValid() const { return data != nullptr; }
    explicit operator bool() const { return IsValid(); }
};

class FontManager
{
public:
    FontManager() = default;
    ~FontManager() { UnloadFont(); }

    // Load a single font from file
    static void LoadFont(const std::string& fontPath, int fontSize = 32);

    // Load the default raylib font (if not already loaded)
    static void LoadDefaultFont();

    // Get the loaded font as FontHandle (no raylib headers needed)
    static FontHandle GetFont() { return fontHandle_; }

    // Check if a font is loaded
    static bool HasFont() { return fontHandle_.IsValid(); }

    // Unload the font
    static void UnloadFont();

    // Get font size
    static int GetFontSize() { return fontSize_; }

private:
    static std::string path_;
    static FontHandle fontHandle_; // Opaque handle to the loaded font (nullptr if not loaded)

    static int fontSize_;
    static bool isDefault_;        // true when falling back to raylib's built-in default font
};

} // namespace RS
