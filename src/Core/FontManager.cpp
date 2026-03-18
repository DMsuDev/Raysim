#include "../pch.hpp"

#include "Raysim/Core/FontManager.hpp"

#include <raylib.h>

//==============================================================================
// Static state
//==============================================================================

namespace RS {
    std::string FontManager::path_       = "";
    FontHandle  FontManager::fontHandle_ = FontHandle();
    int         FontManager::fontSize_   = 0;
    bool        FontManager::isDefault_  = false;
}

void RS::FontManager::LoadDefaultFont()
{
    if (isDefault_ && fontHandle_.IsValid())
        return;

    /// Store a heap-allocated copy of raylib's default font.
    ::Font* font = new ::Font();
    *font = ::GetFontDefault();
    fontHandle_ = FontHandle(static_cast<void*>(font));
    fontSize_ = 20;
    isDefault_ = true;
}

void RS::FontManager::LoadFont(const std::string& fontPath, int fontSize)
{
    RS_ASSERT(!fontPath.empty(), "Font path cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive, got: {}", fontSize);

    LOG_INFO("Loading font from '{}'", fontPath);
    UnloadFont();

    ::Font* font = nullptr;
    bool usingDefault = false;

    std::string resolvedPath = ::GetApplicationDirectory() + fontPath;

    if (::FileExists(resolvedPath.c_str()) == 0)
    {
        LOG_WARN("File '{}' not found, loading default font", resolvedPath);
        font = new ::Font();
        *font = ::GetFontDefault();
        usingDefault = true;
    }
    else
    {
        ::Font* loadedFont = new ::Font();
        *loadedFont = ::LoadFontEx(resolvedPath.c_str(), fontSize, nullptr, 0);

        if (loadedFont->glyphCount == 0)
        {
            LOG_WARN("Failed to load font, loading default font");
            delete loadedFont;
            font = new ::Font();
            *font = ::GetFontDefault();
            usingDefault = true;
        }
        else
        {
            font = loadedFont;
        }
    }

    // Only validate texture for custom fonts (default font texture may not be initialized)
    if (!usingDefault) {
        RS_ASSERT(font->texture.id > 0, "Font texture not properly loaded");
        ::SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
    }

    // Only set fontHandle for custom fonts, use DrawText fallback for default font
    if (!usingDefault) {
        fontHandle_  = FontHandle(static_cast<void*>(font));
        LOG_INFO("Font loaded successfully");
    } else {
        LOG_INFO("Using default font, no custom font handle stored");
        delete font; // Clean up default font as it won't be used
    }

    fontSize_    = fontSize;
    path_        = fontPath;
    isDefault_   = usingDefault;
}

void RS::FontManager::UnloadFont()
{
    if (!HasFont())
    {
        LOG_DEBUG("Attempted to unload font, skipping because no font is loaded");
        return;
    }

    ::Font* font = static_cast<::Font*>(fontHandle_.data);
    RS_ASSERT_NOT_NULL(font);

    if (!isDefault_)
    {
        ::UnloadFont(*font);
        LOG_DEBUG("Font resources freed");
    }
    else
    {
        LOG_DEBUG("Skipping call to ::UnloadFont for default font");
    }

    // Free heap memory regardless of default/copy origin
    delete font;
    fontHandle_ = FontHandle();
    isDefault_  = false;
}
