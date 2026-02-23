#pragma once

// Forward declaration
// NOTE: This is necessary to convert RS::RColor to raylib's Color without including raylib.h in this header
struct Color;

namespace RS {

struct RColor {
    unsigned char r{255}, g{255}, b{255}, a{255};

    RColor() = default;
    constexpr RColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}
    explicit constexpr RColor(unsigned char gray, unsigned char aa = 255)
        : r(gray), g(gray), b(gray), a(aa) {}

    ::Color raylib() const;  // Implemented in Color.cpp

    RColor operator+(const RColor& o) const;
    static RColor lerp(const RColor& a, const RColor& b, float t);
};

// Some predefined colors

constexpr RColor RAYWHITE    {245, 245, 245};
constexpr RColor WHITE       {255, 255, 255};
constexpr RColor BLACK       {  0,   0,   0};
constexpr RColor RED         {255,   0,   0};
constexpr RColor GREEN       {  0, 255,   0};
constexpr RColor BLUE        {  0,   0, 255};
constexpr RColor YELLOW      {255, 255,   0};
constexpr RColor CYAN        {  0, 255, 255};
constexpr RColor MAGENTA     {255,   0, 255};
constexpr RColor ORANGE      {255, 165,   0};
constexpr RColor PINK        {255, 192, 203};
constexpr RColor PURPLE      {128,   0, 128};
constexpr RColor GRAY        {128, 128, 128};
constexpr RColor DARKGRAY    { 64,  64,  64};
constexpr RColor DARKBLUE    { 40,  44,  52};
constexpr RColor LIGHTGRAY   {192, 192, 192};

} // namespace RS
