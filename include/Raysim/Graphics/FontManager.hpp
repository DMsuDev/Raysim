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
    void LoadFont(const std::string& fontPath, int fontSize = 32);

    // Get the loaded font as FontHandle (no raylib headers needed)
    FontHandle GetFont() const { return FontHandle(loaded_ ? font_ : nullptr); }

    // Check if a font is loaded
    bool HasFont() const { return loaded_; }

    // Unload the font
    void UnloadFont();

    // Get font size
    int GetFontSize() const { return loaded_ ? fontSize_ : 0; }

private:
    void* font_      = nullptr;  // raylib Font* (void* to avoid raylib header in hpp)
    int fontSize_    = 32;
    std::string path_;
    bool loaded_     = false;
    bool isDefault_  = false;    // true when falling back to raylib's built-in default font
};

} // namespace RS
