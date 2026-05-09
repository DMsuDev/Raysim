#include "rspch.hpp"

#include "Raysim/Fonts/FontManager.hpp"
#include "Raysim/Utils/PathUtils.hpp"

#include <algorithm>

namespace RS::Fonts {

// ============================================================================
// Static state
// ============================================================================

Scope<TrueTypeProvider> FontManager::s_TextProvider = nullptr;
Scope<FontRenderer> FontManager::s_TextRenderer = nullptr;

std::unordered_map<FontHandle, FontManager::FontEntry> FontManager::s_Fonts;
std::unordered_map<std::string, FontHandle>            FontManager::s_NameMap;

FontHandle FontManager::s_DefaultHandle = DEFAULT_FONT_HANDLE;
FontHandle FontManager::s_NextHandle    = DEFAULT_FONT_HANDLE + 1; // start after the sentinel

// ============================================================================
// Helpers
// ============================================================================

FontHandle FontManager::GenerateHandle() noexcept
{
    return s_NextHandle++;
}

// ============================================================================
// Initialization
// ============================================================================

void FontManager::SetProvider(Scope<TrueTypeProvider> provider)
{
    RS_PROFILE_FUNCTION();
    RS_ASSERT(provider != nullptr, "FontManager: provider must not be null");
    s_TextProvider = std::move(provider);
    RS_CORE_DEBUG("FontManager: TrueType provider set");
}

void FontManager::SetRenderer(Scope<FontRenderer> renderer)
{
    RS_PROFILE_FUNCTION();
    RS_ASSERT(renderer != nullptr, "FontManager: renderer must not be null");
    s_TextRenderer = std::move(renderer);
    s_TextRenderer->Init();
    RS_CORE_DEBUG("FontManager: font renderer set and initialized");
}

void FontManager::Shutdown()
{
    RS_PROFILE_FUNCTION();
    UnloadAll();

    if (s_TextRenderer) {
        // s_TextRenderer->Shutdown() is called on reset via the destructor.
        s_TextRenderer.reset();
    }

    s_TextProvider.reset();
    s_NextHandle    = DEFAULT_FONT_HANDLE + 1;
    s_DefaultHandle = DEFAULT_FONT_HANDLE;

    RS_CORE_DEBUG("FontManager: shut down");
}

// ============================================================================
// Loading
// ============================================================================

FontHandle FontManager::LoadFont(
    const std::string& name,
    const std::string& path,
    int                fontSize,
    uint32_t           firstChar,
    uint32_t           charCount)
{
    RS_PROFILE_FUNCTION();

    RS_ASSERT(!name.empty(), "FontManager::LoadFont -> name cannot be empty");
    RS_ASSERT(fontSize > 0,  "FontManager::LoadFont -> fontSize must be positive (got {})", fontSize);

    RS_CORE_DEBUG("FontManager::LoadFont -> loading '{}' from '{}' (size={}px, chars=[{}-{}])",
                  name, path, fontSize, firstChar, firstChar + charCount - 1);

    // -- Cache hit ---------------------------------------------------------
    if (const auto it = s_NameMap.find(name); it != s_NameMap.end()) {
        RS_CORE_DEBUG("FontManager: '{}' already loaded (cached)", name);
        return it->second;
    }

    if (!s_TextProvider) {
        RS_CORE_ERROR("FontManager::LoadFont -> no TrueTypeProvider set. Call SetProvider() first.");
        return INVALID_FONT_HANDLE;
    }

    if (!s_TextRenderer) {
        RS_CORE_ERROR("FontManager::LoadFont -> no FontRenderer set. Call SetRenderer() first.");
        return INVALID_FONT_HANDLE;
    }

    // -- Resolve path ------------------------------------------------------
    const std::string resolved = Utils::ResolvePathStr(path, Utils::PathBase::Assets);

    // -- Load via provider -------------------------------------------------
    FontEntry entry;
    entry.font.SetName(name);
    entry.handle = GenerateHandle();

    if (!s_TextProvider->LoadFont(resolved, fontSize, firstChar, charCount, entry.font)) {
        RS_CORE_ERROR("FontManager::LoadFont -> failed to load font '{}' from '{}'", name, resolved);
        return INVALID_FONT_HANDLE;
    }

    // -- Upload to GPU -----------------------------------------------------
    if (!s_TextRenderer->UploadFontAtlas(entry.handle, entry.font.GetAtlas())) {
        RS_CORE_ERROR("FontManager::LoadFont -> GPU upload failed for font '{}'", name);
        return INVALID_FONT_HANDLE;
    }

    // -- Store -------------------------------------------------------------
    const FontHandle h = entry.handle;
    s_Fonts[h] = std::move(entry);
    s_NameMap[name] = h;

    // Auto-promote: the first TTF named "default" becomes the active default,
    // replacing the built-in sentinel.  Subsequent LoadFont calls won't reach
    // here (cache hit above), so s_DefaultHandle is only set once.
    if (name == "default" || s_DefaultHandle == DEFAULT_FONT_HANDLE)
        s_DefaultHandle = h;

    RS_CORE_DEBUG("FontManager::LoadFont -> loaded '{}' (handle={}, size={}px)", name, h, fontSize);
    return h;
}

FontHandle FontManager::LoadDefaultFont()
{
    RS_PROFILE_FUNCTION();

    // Re-register the built-in default font entry (no provider / atlas needed;
    // RaylibFontRenderer handles DEFAULT_FONT_HANDLE internally via GetFontDefault()).
    // Use "__builtin__" as internal name so user-registered "default" doesn't conflict.
    if (const auto it = s_NameMap.find("__builtin__"); it != s_NameMap.end())
        return it->second;

    if (const auto it = s_NameMap.find("default"); it != s_NameMap.end())
        return it->second;

    FontEntry entry;
    entry.font.SetName("__builtin__");
    entry.font.SetLoaded(true);
    entry.font.SetDefault(true);
    entry.font.SetSize(20);
    entry.handle = DEFAULT_FONT_HANDLE;

    s_Fonts[DEFAULT_FONT_HANDLE] = std::move(entry);
    s_NameMap["__builtin__"]     = DEFAULT_FONT_HANDLE;
    // s_DefaultHandle stays at DEFAULT_FONT_HANDLE until the user loads a real TTF.

    RS_CORE_DEBUG("FontManager::LoadDefaultFont -> built-in font registered");
    return DEFAULT_FONT_HANDLE;
}

// ============================================================================
// Access
// ============================================================================

const Font* FontManager::GetFont(FontHandle handle)
{
    const auto it = s_Fonts.find(handle);
    return (it != s_Fonts.end()) ? &it->second.font : nullptr;
}

const Font* FontManager::GetFontByName(const std::string& name)
{
    return GetFont(GetFontHandle(name));
}

FontHandle FontManager::GetFontHandle(const std::string& name)
{
    const auto it = s_NameMap.find(name);
    return (it != s_NameMap.end()) ? it->second : INVALID_FONT_HANDLE;
}

FontHandle FontManager::GetDefaultHandle()
{
    return s_DefaultHandle;
}

bool FontManager::HasFont(FontHandle handle)
{
    return handle != INVALID_FONT_HANDLE && s_Fonts.count(handle) > 0;
}

bool FontManager::HasFont(const std::string& name)
{
    return s_NameMap.count(name) > 0;
}

std::vector<std::string> FontManager::GetLoadedFontNames()
{
    std::vector<std::string> names;
    names.reserve(s_NameMap.size());
    for (const auto& [name, _] : s_NameMap)
        names.push_back(name);
    std::sort(names.begin(), names.end());
    return names;
}

// ============================================================================
// Unloading
// ============================================================================

void FontManager::UnloadFont(FontHandle handle)
{
    if (handle == INVALID_FONT_HANDLE) return;

    const auto it = s_Fonts.find(handle);
    if (it == s_Fonts.end()) {
        RS_CORE_WARN("FontManager::UnloadFont -> handle {} not found", handle);
        return;
    }

    const bool wasDefault = (handle == s_DefaultHandle);

    // Release GPU texture (skip for default sentinel which has no real atlas)
    if (s_TextRenderer && !it->second.font.IsDefault())
        s_TextRenderer->ReleaseFontAtlas(handle);

    // Remove from name map
    s_NameMap.erase(it->second.font.GetName());
    s_Fonts.erase(it);

    if (wasDefault)
        s_DefaultHandle = DEFAULT_FONT_HANDLE;

    RS_CORE_DEBUG("FontManager::UnloadFont -> unloaded handle {}", handle);
}

void FontManager::UnloadFont(const std::string& name)
{
    const auto it = s_NameMap.find(name);
    if (it == s_NameMap.end()) {
        RS_CORE_WARN("FontManager::UnloadFont -> '{}' not found", name);
        return;
    }
    UnloadFont(it->second);
}

void FontManager::UnloadAll()
{
    // Collect handles so we don't invalidate iterators mid-loop
    std::vector<FontHandle> handles;
    handles.reserve(s_Fonts.size());
    for (const auto& [h, _] : s_Fonts)
        handles.push_back(h);

    for (FontHandle h : handles)
        UnloadFont(h);

    s_DefaultHandle = DEFAULT_FONT_HANDLE;

    RS_CORE_DEBUG("FontManager::UnloadAll -> all fonts unloaded");
}

// ============================================================================
// Metrics
// ============================================================================

Math::Vec2f FontManager::MeasureText(
    FontHandle         handle,
    const std::string& text,
    float              spacing)
{
    // Resolve handle
    if (handle == INVALID_FONT_HANDLE || handle == DEFAULT_FONT_HANDLE) {
        // Fallback: delegate to renderer which knows about the built-in font
        if (s_TextRenderer)
            return s_TextRenderer->MeasureDefaultText(text, 20.0f);
        return Math::Vec2f{ static_cast<float>(text.size()) * 10.0f, 20.0f };
    }

    if (!s_TextProvider) return {0.0f, 0.0f};

    const Font* font = GetFont(handle);
    if (!font) return {0.0f, 0.0f};

    return s_TextProvider->MeasureText(*font, text, spacing);
}

// ============================================================================
// Rendering
// ============================================================================

void FontManager::RenderText(
    FontHandle         handle,
    const std::string& text,
    float              x,
    float              y,
    float              fontSize,
    float              spacing,
    const Color&       color)
{
    RS_ASSERT(!text.empty(), "FontManager::RenderText -> text cannot be empty");
    RS_ASSERT(fontSize > 0,  "FontManager::RenderText -> fontSize must be positive (got {})", fontSize);

    if (!s_TextRenderer) {
        RS_CORE_WARN("FontManager::RenderText -> no renderer set, skipping draw");
        return;
    }

    // Resolve to default if needed
    const FontHandle resolvedHandle =
        (handle == INVALID_FONT_HANDLE) ? s_DefaultHandle : handle;

    const Font* font = GetFont(resolvedHandle);
    // Default font entry may have an empty atlas, renderer handles it
    static const Font kEmptyFont{};
    const Font& fontRef = font ? *font : kEmptyFont;

    s_TextRenderer->RenderText(resolvedHandle, fontRef, text, x, y, fontSize, spacing, color);
}

} // namespace RS::Fonts
