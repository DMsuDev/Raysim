#pragma once

namespace RS {

struct Color {
    unsigned char r{255}, g{255}, b{255}, a{255};

    Color() = default;
    constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}
    explicit constexpr Color(unsigned char gray, unsigned char aa = 255)
        : r(gray), g(gray), b(gray), a(aa) {}

    Color operator+(const Color& o) const;

    static Color lerp(const Color& a, const Color& b, float t);
};

constexpr Color RAYWHITE    {245, 245, 245};
constexpr Color WHITE       {255, 255, 255};
constexpr Color BLACK       {  0,   0,   0};
constexpr Color RED         {255,   0,   0};
constexpr Color GREEN       {  0, 255,   0};
constexpr Color BLUE        {  0,   0, 255};
constexpr Color YELLOW      {255, 255,   0};
constexpr Color CYAN        {  0, 255, 255};
constexpr Color MAGENTA     {255,   0, 255};
constexpr Color ORANGE      {255, 165,   0};
constexpr Color PINK        {255, 192, 203};
constexpr Color PURPLE      {128,   0, 128};
constexpr Color GRAY        {128, 128, 128};
constexpr Color DARKGRAY    { 64,  64,  64};
constexpr Color DARKBLUE    { 40,  44,  52};
constexpr Color LIGHTGRAY   {192, 192, 192};

} // namespace RS
