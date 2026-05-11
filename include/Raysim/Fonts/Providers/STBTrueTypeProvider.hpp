/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

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

    Math::Vec2f MeasureText(
        const Font&        font,
        const std::string& text,
        float              spacing = 0.0f) const override;

private:
    /// Choose atlas dimensions based on fontSize.
    static void ChooseAtlasSize(int fontSize, uint32_t& outW, uint32_t& outH) noexcept;
};

} // namespace RS::Fonts
