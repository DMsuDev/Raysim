#include "../pch.hpp"

#include "Raysim/Manager/FontManager.hpp"

#include <raylib.h>

// Static member definitions
namespace RS {
    std::string FontManager::path_      = "";
    void*       FontManager::font_      = nullptr;
    int         FontManager::fontSize_  = 0;
    bool        FontManager::loaded_    = false;
    bool        FontManager::isDefault_ = false;
}

void RS::FontManager::LoadFont(const std::string& fontPath, int fontSize)
{
    RS_ASSERT(!fontPath.empty(), "Font path cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive, got: {}", fontSize);

    LOG_INFO("FontManager: Loading font from '{}' with size {}", fontPath, fontSize);
    UnloadFont();

    ::Font* font = new ::Font();
    RS_ASSERT_NOT_NULL(font);
    bool usingDefault = false;

    if (::FileExists(fontPath.c_str()) == 0)
    {
        LOG_WARN("FontManager: Font file not found at '{}', falling back to default font", fontPath);
        *font = ::GetFontDefault();
        usingDefault = true;
    }
    else
    {
        LOG_DEBUG("FontManager: File exists, attempting to load...");
        *font = ::LoadFontEx(fontPath.c_str(), fontSize, nullptr, 0);

        if (font->glyphCount == 0) {
            LOG_WARN("FontManager: Failed to load font at '{}' (glyph count: 0), falling back to default font", fontPath);
            delete font;
            font = new ::Font();
            *font = ::GetFontDefault();
            usingDefault = true;
        }
    }

    if (usingDefault) {
        LOG_INFO("FontManager: Using default raylib font (size: {})", fontSize);
    } else {
        LOG_INFO("FontManager: Successfully loaded font '{}' (size: {}, glyphs: {})", fontPath, fontSize, font->glyphCount);
    }
    RS_ASSERT(font->texture.id > 0, "Font texture not properly loaded");
    ::SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);

    font_      = font;
    fontSize_  = fontSize;
    path_      = fontPath;
    loaded_    = true;
    isDefault_ = usingDefault;

    RS_ASSERT(loaded_, "Font should be marked as loaded after LoadFont");
}

void RS::FontManager::UnloadFont()
{
    if (loaded_ && font_ != nullptr)
    {
        LOG_DEBUG("FontManager: Unloading font from '{}'", path_);
        ::Font* font = RS::Cast<::Font*>(font_);
        RS_ASSERT_NOT_NULL(font);
        // Do not call ::UnloadFont on raylib's built-in default font — it's static
        if (!isDefault_)
        {
            ::UnloadFont(*font);
            LOG_DEBUG("FontManager: Font resources freed");
        }
        else {
            LOG_DEBUG("FontManager: Skipping unload of default font");
        }
        delete font;
        font_      = nullptr;
        loaded_    = false;
        isDefault_ = false;
    }
    else if (loaded_)
    {
        LOG_WARN("FontManager: Attempted to unload font, but font pointer is null");
    }
}
