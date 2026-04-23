#include "pch.hpp"

#include "Raysim/Graphics/Shapes.hpp"

#include <raylib.h>

namespace RS {

namespace {
    /// Convert `RS::Color` to raylib's `Color`.
    [[nodiscard]] constexpr ::Color ToRL(const Color& c) noexcept
    {
        return ::Color{ c.r, c.g, c.b, c.a };
    }

    /// Convert `OriginMode` to a normalized origin vector in [0..1].
    [[nodiscard]] constexpr ::Vector2 OriginToVector(OriginMode origin) noexcept
    {
        float x = 0.0f, y = 0.0f;

        switch (origin) {
            case OriginMode::TopLeft:      x = 0.0f; y = 0.0f; break;
            case OriginMode::TopCenter:    x = 0.5f; y = 0.0f; break;
            case OriginMode::TopRight:     x = 1.0f; y = 0.0f; break;
            case OriginMode::CenterLeft:   x = 0.0f; y = 0.5f; break;
            case OriginMode::Center:       x = 0.5f; y = 0.5f; break;
            case OriginMode::CenterRight:  x = 1.0f; y = 0.5f; break;
            case OriginMode::BottomLeft:   x = 0.0f; y = 1.0f; break;
            case OriginMode::BottomCenter: x = 0.5f; y = 1.0f; break;
            case OriginMode::BottomRight:  x = 1.0f; y = 1.0f; break;
            default:                       x = 0.0f; y = 0.0f; break;
        }

        return { x, y };
    }
} // namespace

void Shapes::DrawRect(float x, float y, float w, float h, const Color& color) {
    ::DrawRectangle(static_cast<int>(x), static_cast<int>(y),
                    static_cast<int>(w), static_cast<int>(h), ToRL(color));
}

void Shapes::DrawRectOutline(float x, float y, float w, float h, const Color& color, float thickness) {
    RS_ASSERT(w > 0 && h > 0, "Rectangle dimensions must be positive");
    RS_ASSERT(thickness > 0, "Line thickness must be positive: {}", thickness);
    ::Rectangle rec = {x, y, w, h};
    ::DrawRectangleLinesEx(rec, thickness, ToRL(color));
}

void Shapes::DrawRectRounded(float x, float y, float w, float h, float roundness, const Color& color) {
    ::Rectangle rec = {x, y, w, h};
    ::DrawRectangleRounded(rec, roundness, 8, ToRL(color));
}

//==============================================================================
// Circle
//==============================================================================

void Shapes::DrawCircle(float x, float y, float radius, const Color& color) {
    ::DrawCircle(static_cast<int>(x), static_cast<int>(y), radius, ToRL(color));
}

void Shapes::DrawCircleOutline(float x, float y, float radius, const Color& color) {
    ::DrawCircleLines(static_cast<int>(x), static_cast<int>(y), radius, ToRL(color));
}

//==============================================================================
// Line
//==============================================================================

void Shapes::DrawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness) {
    RS_ASSERT(thickness > 0, "Line thickness must be positive: {}", thickness);
    if (thickness <= 1.0f) {
        ::DrawLine(static_cast<int>(x1), static_cast<int>(y1),
                   static_cast<int>(x2), static_cast<int>(y2), ToRL(color));
    } else {
        ::DrawLineEx({x1, y1}, {x2, y2}, thickness, ToRL(color));
    }
}

//==============================================================================
// Triangle
//==============================================================================

void Shapes::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const Color& color) {
    ::DrawTriangle({x1, y1}, {x2, y2}, {x3, y3}, ToRL(color));
}

void Shapes::DrawTriangleOutline(float x1, float y1, float x2, float y2, float x3, float y3, const Color& color) {
    ::DrawTriangleLines({x1, y1}, {x2, y2}, {x3, y3}, ToRL(color));
}

//==============================================================================
// Polygon
//==============================================================================

void Shapes::DrawPolygon(float x, float y, int sides, float radius, float rotation, const Color& color) {
    RS_ASSERT(sides >= 3, "Polygon must have at least 3 sides, got: {}", sides);
    RS_ASSERT(radius > 0, "Polygon radius must be positive: {}", radius);
    ::Vector2 center = {x, y};
    ::DrawPoly(center, sides, radius, rotation, ToRL(color));
}

//==============================================================================
// Point
//==============================================================================

void Shapes::DrawPoint(float x, float y, const Color& color) {
    ::DrawPixel(static_cast<int>(x), static_cast<int>(y), ToRL(color));
}

void Shapes::DrawPoint(float x, float y, float size, const Color& color) {
    RS_ASSERT(size > 0, "Point size must be positive: {}", size);
    ::DrawCircle(static_cast<int>(x), static_cast<int>(y), size, ToRL(color));
}

//==============================================================================
// Rectangle (origin mode)
//==============================================================================

void Shapes::DrawRect(float x, float y, float w, float h, const Color& color, OriginMode origin) {
    ::Rectangle rec = {x, y, w, h};
    ::Vector2 originVec = OriginToVector(origin);
    ::DrawRectanglePro(rec, {originVec.x * w, originVec.y * h}, 0.0f, ToRL(color));
}

void Shapes::DrawRectOutline(float x, float y, float w, float h, const Color& color, float thickness, OriginMode origin) {
    ::Vector2 originVec = OriginToVector(origin);
    // Calculate the adjusted top-left position for DrawRectangleLinesEx
    float adjustedX = x - (originVec.x * w);
    float adjustedY = y - (originVec.y * h);
    ::Rectangle adjustedRec = {adjustedX, adjustedY, w, h};
    ::DrawRectangleLinesEx(adjustedRec, thickness, ToRL(color));
}

void Shapes::DrawRectRounded(float x, float y, float w, float h, float roundness, const Color& color, OriginMode origin) {
    ::Vector2 originVec = OriginToVector(origin);
    // Calculate the adjusted top-left position
    float adjustedX = x - (originVec.x * w);
    float adjustedY = y - (originVec.y * h);
    ::Rectangle rec = {adjustedX, adjustedY, w, h};
    ::DrawRectangleRounded(rec, roundness, 8, ToRL(color));
}

//==============================================================================
// Circle (origin mode)
//==============================================================================

void Shapes::DrawCircle(float x, float y, float radius, const Color& color, OriginMode origin) {
    ::Vector2 originVec = OriginToVector(origin);
    // For circles, origin is calculated based on bounding box (radius * 2)
    float adjustedX = x - (originVec.x * radius * 2.0f) + radius;
    float adjustedY = y - (originVec.y * radius * 2.0f) + radius;
    ::DrawCircle(static_cast<int>(adjustedX), static_cast<int>(adjustedY), radius, ToRL(color));
}

void Shapes::DrawCircleOutline(float x, float y, float radius, const Color& color, OriginMode origin) {
    ::Vector2 originVec = OriginToVector(origin);
    // For circles, origin is calculated based on bounding box (radius * 2)
    float adjustedX = x - (originVec.x * radius * 2.0f) + radius;
    float adjustedY = y - (originVec.y * radius * 2.0f) + radius;
    ::DrawCircleLines(static_cast<int>(adjustedX), static_cast<int>(adjustedY), radius, ToRL(color));
}

} // namespace RS
