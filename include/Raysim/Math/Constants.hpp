#pragma once

#ifdef PI
#undef PI
#endif

#ifdef TWO_PI
#undef TWO_PI
#endif

namespace RS {

    constexpr float PI = 3.14159265358979f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = PI / 2.0f;
    constexpr float QUARTER_PI = PI / 4.0f;
    constexpr float TAU = TWO_PI;
    constexpr float E = 2.71828182845904f;

}
