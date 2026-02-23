#include "RaySim/Graphics/FontManager.hpp"
#include "pch.hpp"

#include <raylib.h>

void RS::FontManager::LoadFont(const std::string& fontPath, int fontSize)
{
    // Unload current font if already loaded
    UnloadFont();

    ::Font* font = new ::Font();
    bool usingDefault = false;

    if (::FileExists(fontPath.c_str()) == 0)
    {
        ::TraceLog(LOG_WARNING, "RaySim: Font not found at '%s', falling back to default font", fontPath.c_str());
        *font = ::GetFontDefault();
        usingDefault = true;
    }
    else
    {
        *font = ::LoadFontEx(fontPath.c_str(), fontSize, nullptr, 0);

        if (font->glyphCount == 0) {
            ::TraceLog(LOG_WARNING, "RaySim: Failed to load font at '%s', falling back to default font", fontPath.c_str());
            delete font;
            font = new ::Font();
            *font = ::GetFontDefault();
            usingDefault = true;
        }
    }

    ::TraceLog(LOG_INFO, "RaySim: Loaded font '%s' (size: %d)", fontPath.c_str(), fontSize);
    ::SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);

    font_      = font;
    fontSize_  = fontSize;
    path_      = fontPath;
    loaded_    = true;
    isDefault_ = usingDefault;
}

void RS::FontManager::UnloadFont()
{
    if (loaded_ && font_ != nullptr)
    {
        ::Font* font = static_cast<::Font*>(font_);
        // Do not call ::UnloadFont on raylib's built-in default font — it's static
        if (!isDefault_)
        {
            ::UnloadFont(*font);
        }
        delete font;
        font_      = nullptr;
        loaded_    = false;
        isDefault_ = false;
    }
    else if (loaded_)
    {
        ::TraceLog(LOG_WARNING, "RaySim: Attempted to unload font, but no font was loaded");
    }
}
