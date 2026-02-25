#pragma once

#include <string>

namespace RS {

/// Opaque handle for raylib Font without exposing raylib headers
struct FontHandle {
    void* data = nullptr;

    FontHandle() = default;
    explicit FontHandle(void* ptr) : data(ptr) {}

    bool IsValid() const { return data != nullptr; }
    explicit operator bool() const { return IsValid(); }
};

/// FontManager - manages font loading and lifecycle
/// Provides a static interface to load, unload and access fonts
class FontManager
{
public:
    FontManager() = default;
    ~FontManager() { UnloadFont(); }

    // Load a single font from file
    static void LoadFont(const std::string& fontPath, int fontSize = 32);

    // Get the loaded font as FontHandle (no raylib headers needed)
    static FontHandle GetFont() { return FontHandle(loaded_ ? font_ : nullptr); }

    // Check if a font is loaded
    static bool HasFont() { return loaded_; }

    // Unload the font
    static void UnloadFont();

    // Get font size
    static int GetFontSize() { return loaded_ ? fontSize_ : 0; }

private:
    static std::string path_;
    static void* font_;      // raylib Font* (void* to avoid raylib header in hpp)
    static int fontSize_;
    static bool loaded_;
    static bool isDefault_;    // true when falling back to raylib's built-in default font
};

} // namespace RS
