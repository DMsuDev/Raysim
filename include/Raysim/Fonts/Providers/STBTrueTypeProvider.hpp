#pragma once

#include "TrueTypeProvider.hpp"

namespace RS::Fonts {

class STBTrueTypeProvider final : public TrueTypeProvider {
public:
    STBTrueTypeProvider()                                       = default;
    ~STBTrueTypeProvider()                                      = default;
    STBTrueTypeProvider(const STBTrueTypeProvider&)             = delete;
    STBTrueTypeProvider& operator=(const STBTrueTypeProvider&)  = delete;

    bool LoadFont(
        const std::string& path,
        int                fontSize,
        uint32_t           firstChar,
        uint32_t           charCount,
        Font&              outFont) override;

    Math::Vec2 MeasureText(
        const Font&        font,
        const std::string& text,
        float              spacing = 0.0f) const override;

private:
    /// Choose atlas dimensions based on fontSize.
    static void ChooseAtlasSize(int fontSize, uint32_t& outW, uint32_t& outH) noexcept;
};

} // namespace RS::Fonts
