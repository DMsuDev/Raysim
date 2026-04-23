#include "pch.hpp"

#include "Raysim/Math/Random.hpp"

#include <cmath>
#include <algorithm>
#include <limits>
#include <random>

namespace RS::Math::Random {

//==============================================================================
// Internal state
//==============================================================================

// Seeded from hardware RNG so every launch is unique by default.
static std::random_device rd;
static std::mt19937       generator(rd());

// Noise seed - mixed into every hash so Seed() changes the noise landscape.
static unsigned int noiseSeed = rd();

namespace {
[[nodiscard]] float ExclusiveUpperBound(float max) noexcept
{
    if (!std::isfinite(max)) return max;
    return std::nextafter(max, -std::numeric_limits<float>::infinity());
}
} // namespace

//==============================================================================
// Seeding
//==============================================================================

void Seed(unsigned int seed)
{
    generator.seed(seed);
    noiseSeed = seed;
}

void SeedRandom()
{
    unsigned int s = rd();
    generator.seed(s);
    noiseSeed = s;
}

unsigned int GetNoiseSeed()
{
    return noiseSeed;
}

//==============================================================================
// Random number generation
//==============================================================================

int Int(int max) {
    if (max <= 0) return 0;
    std::uniform_int_distribution<int> distribution(0, max - 1);
    return distribution(generator);
}

int Range(int min, int max) {
    if (min >= max) return min;
    std::uniform_int_distribution<int> distribution(min, max - 1);
    return distribution(generator);
}

float Float() {
    std::uniform_real_distribution<float> distribution(0.0f, ExclusiveUpperBound(1.0f));
    return distribution(generator);
}

float Float(float max) {
    if (max <= 0.0f) return 0.0f;
    std::uniform_real_distribution<float> distribution(0.0f, ExclusiveUpperBound(max));
    return distribution(generator);
}

float Range(float min, float max) {
    if (min >= max) return min;
    std::uniform_real_distribution<float> distribution(min, ExclusiveUpperBound(max));
    return distribution(generator);
}

bool Bool() {
    return Int(2) == 0;
}

bool Bool(float probability) {
    probability = (probability < 0.0f) ? 0.0f : (probability > 1.0f) ? 1.0f : probability;
    return Float() < probability;
}

//==============================================================================
// Noise - internal hash helpers (incorporate noiseSeed)
//==============================================================================

static int HashInt(int v)
{
    v ^= static_cast<int>(noiseSeed);
    v = (v ^ 61) ^ (v >> 16);
    v *= 0x27d4eb2d;
    v ^= v >> 15;
    return v & 0x7fffffff;
}

static float HashFloat(int v)
{
    return static_cast<float>(HashInt(v)) / static_cast<float>(0x7fffffff);
}

static int Hash2DInt(int x, int y)
{
    return HashInt((x * 73856093) ^ (y * 19349663));
}

static float Hash2DFloat(int x, int y)
{
    return HashFloat((x * 73856093) ^ (y * 19349663));
}

// Separate hash for the Y component of cell feature points so X and Y are independent.
static float Hash2DFloatB(int x, int y)
{
    return HashFloat((x * 83492791) ^ (y * 48611));
}

static int Hash3DInt(int x, int y, int z)
{
    return HashInt((x * 73856093) ^ (y * 19349663) ^ (z * 83492791));
}

/// Quintic interpolation (6t^5 - 15t^4 + 10t^3) - smoother than 3t^2 - 2t^3.
static float Fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/// Dot product of a pseudo-random gradient and distance vector.
static float GradientDot2D(int hash, float dx, float dy)
{
    // 8 evenly-spaced unit gradients
    static constexpr float grads[8][2] = {
        { 1, 0}, { 0, 1}, {-1, 0}, { 0,-1},
        { 1, 1}, {-1, 1}, { 1,-1}, {-1,-1}
    };
    int h = hash & 7;
    return grads[h][0] * dx + grads[h][1] * dy;
}

//==============================================================================
// Perlin noise 2D
//==============================================================================

float PerlinNoise(float x, float y)
{
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - static_cast<float>(xi);
    float yf = y - static_cast<float>(yi);

    float u = Fade(xf);
    float v = Fade(yf);

    float n00 = GradientDot2D(Hash2DInt(xi,     yi    ), xf,        yf       );
    float n10 = GradientDot2D(Hash2DInt(xi + 1, yi    ), xf - 1.0f, yf       );
    float n01 = GradientDot2D(Hash2DInt(xi,     yi + 1), xf,        yf - 1.0f);
    float n11 = GradientDot2D(Hash2DInt(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

    float nx0 = n00 + u * (n10 - n00);
    float nx1 = n01 + u * (n11 - n01);
    return nx0 + v * (nx1 - nx0);
}

//==============================================================================
// Perlin noise 3D
//==============================================================================

float PerlinNoise(float x, float y, float z)
{
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    int zi = static_cast<int>(std::floor(z));
    float xf = x - static_cast<float>(xi);
    float yf = y - static_cast<float>(yi);
    float zf = z - static_cast<float>(zi);

    float u = Fade(xf);
    float v = Fade(yf);
    float w = Fade(zf);

    auto grad3d = [](int hash, float dx, float dy, float dz) {
        int h = hash & 15;
        float gu = (h < 8) ? dx : dy;
        float gv = (h < 4) ? dy : (h == 12 || h == 14) ? dx : dz;
        return ((h & 1) ? -gu : gu) + ((h & 2) ? -gv : gv);
    };

    float n000 = grad3d(Hash3DInt(xi,     yi,     zi    ), xf,        yf,        zf       );
    float n100 = grad3d(Hash3DInt(xi + 1, yi,     zi    ), xf - 1.0f, yf,        zf       );
    float n010 = grad3d(Hash3DInt(xi,     yi + 1, zi    ), xf,        yf - 1.0f, zf       );
    float n110 = grad3d(Hash3DInt(xi + 1, yi + 1, zi    ), xf - 1.0f, yf - 1.0f, zf       );
    float n001 = grad3d(Hash3DInt(xi,     yi,     zi + 1), xf,        yf,        zf - 1.0f);
    float n101 = grad3d(Hash3DInt(xi + 1, yi,     zi + 1), xf - 1.0f, yf,        zf - 1.0f);
    float n011 = grad3d(Hash3DInt(xi,     yi + 1, zi + 1), xf,        yf - 1.0f, zf - 1.0f);
    float n111 = grad3d(Hash3DInt(xi + 1, yi + 1, zi + 1), xf - 1.0f, yf - 1.0f, zf - 1.0f);

    float nx00 = n000 + u * (n100 - n000);
    float nx10 = n010 + u * (n110 - n010);
    float nx01 = n001 + u * (n101 - n001);
    float nx11 = n011 + u * (n111 - n011);

    float nxy0 = nx00 + v * (nx10 - nx00);
    float nxy1 = nx01 + v * (nx11 - nx01);

    return nxy0 + w * (nxy1 - nxy0);
}

//==============================================================================
// Simplex noise 2D (proper triangle-lattice implementation)
//==============================================================================

float SimplexNoise(float x, float y)
{
    // Skew constants for 2D simplex grid
    constexpr float F2 = 0.3660254037844386f;  // (sqrt(3) - 1) / 2
    constexpr float G2 = 0.21132486540518713f; // (3 - sqrt(3)) / 6

    // Skew input space to determine which simplex cell we're in
    float s  = (x + y) * F2;
    int   i  = static_cast<int>(std::floor(x + s));
    int   j  = static_cast<int>(std::floor(y + s));

    // Unskew back to (x,y) space for the cell origin
    float unskew  = static_cast<float>(i + j) * G2;
    float x0 = x - (static_cast<float>(i) - unskew);
    float y0 = y - (static_cast<float>(j) - unskew);

    // Determine which simplex triangle we're in
    int i1, j1;
    if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle
    else          { i1 = 0; j1 = 1; } // upper triangle

    float x1 = x0 - static_cast<float>(i1) + G2;
    float y1 = y0 - static_cast<float>(j1) + G2;
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Contribution from the three corners
    auto contrib = [](int hash, float cx, float cy) -> float {
        float t = 0.5f - cx * cx - cy * cy;
        if (t < 0.0f) return 0.0f;
        t *= t;
        return t * t * GradientDot2D(hash, cx, cy);
    };

    float n0 = contrib(Hash2DInt(i,      j     ), x0, y0);
    float n1 = contrib(Hash2DInt(i + i1, j + j1), x1, y1);
    float n2 = contrib(Hash2DInt(i + 1,  j + 1 ), x2, y2);

    // Scale to approximately [-1, 1]
    return 70.0f * (n0 + n1 + n2);
}

//==============================================================================
// Cellular (Worley) noise 2D
//==============================================================================

float CellularNoise(float x, float y)
{
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - static_cast<float>(xi);
    float yf = y - static_cast<float>(yi);

    float minDist = 2.0f;

    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            int cx = xi + di;
            int cy = yi + dj;
            // Independent hashes for X and Y so feature points aren't on the diagonal
            float fpx = static_cast<float>(di) + Hash2DFloat (cx, cy);
            float fpy = static_cast<float>(dj) + Hash2DFloatB(cx, cy);

            float dx = fpx - xf;
            float dy = fpy - yf;
            float dist = std::sqrt(dx * dx + dy * dy);
            minDist = std::min(minDist, dist);
        }
    }

    return std::clamp(minDist, 0.0f, 1.0f);
}

//==============================================================================
// Value noise 2D
//==============================================================================

float ValueNoise(float x, float y)
{
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - static_cast<float>(xi);
    float yf = y - static_cast<float>(yi);

    float u = Fade(xf);
    float v = Fade(yf);

    float v00 = Hash2DFloat(xi,     yi    );
    float v10 = Hash2DFloat(xi + 1, yi    );
    float v01 = Hash2DFloat(xi,     yi + 1);
    float v11 = Hash2DFloat(xi + 1, yi + 1);

    float vx0 = v00 + u * (v10 - v00);
    float vx1 = v01 + u * (v11 - v01);
    return vx0 + v * (vx1 - vx0);
}

//==============================================================================
// Fractal Brownian Motion
//==============================================================================

float FractalBrownianMotion(float x, float y, int octaves, float persistence, float lacunarity)
{
    float result    = 0.0f;
    float amplitude = 1.0f;
    float maxValue  = 0.0f;
    float freq      = 1.0f;

    for (int i = 0; i < octaves; ++i) {
        result   += PerlinNoise(x * freq, y * freq) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        freq      *= lacunarity;
    }

    return result / maxValue;
}

} // namespace RS::Math::Random
