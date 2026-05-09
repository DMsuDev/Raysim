#include "pch.hpp"

#include "Raysim/Math/Random.hpp"
#include "Raysim/Math/Utils/MathUtils.hpp"

#include <cstdint>
#include <random>

namespace RS::Math::Random {

//==============================================================================
// Internal state
//==============================================================================

// Seeded from hardware RNG so every launch is unique by default.
// std::random_device is used transiently, not for ongoing generation, to avoid performance overhead.
// The Mersenne Twister engine is used for fast generation after seeding.
static std::mt19937 generator(std::random_device{}());

// Noise seed mixed into every hash so Seed() changes the noise landscape.
static uint32_t noiseSeed = std::random_device{}();

//==============================================================================
// Seeding
//==============================================================================

void Seed(unsigned int seed)
{
    RS_CORE_TRACE("Random: seeded with {}", seed);
    generator.seed(seed);
    noiseSeed = seed;
}

void SeedRandom()
{
    const uint32_t s = std::random_device{}();
    RS_CORE_TRACE("Random: seeded with hardware entropy ({})", s);
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
    // uniform_real_distribution guarantees [a, b) by the standard.
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}

float Float(float max) {
    if (max <= 0.0f) return 0.0f;
    std::uniform_real_distribution<float> distribution(0.0f, max);
    return distribution(generator);
}

float Range(float min, float max) {
    if (min >= max) return min;
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

bool Bool() {
    return Int(2) == 0;
}

bool Bool(float probability) {
    return Float() < Clamp(probability, 0.0f, 1.0f);
}

//==============================================================================
// Noise - internal hash helpers (incorporate noiseSeed)
//==============================================================================

// All arithmetic is in uint32_t to avoid signed-integer-overflow UB.
static int HashInt(int v)
{
    uint32_t h = static_cast<uint32_t>(v) ^ noiseSeed;
    h = (h ^ 61u) ^ (h >> 16u);
    h *= 0x27d4eb2du;
    h ^= h >> 15u;
    return static_cast<int>(h & 0x7fffffffu);
}

static float HashFloat(int v)
{
    return static_cast<float>(HashInt(v)) / static_cast<float>(0x7fffffff);
}

static int Hash2DInt(int x, int y)
{
    return HashInt(static_cast<int>(
        static_cast<uint32_t>(x) * 73856093u ^
        static_cast<uint32_t>(y) * 19349663u));
}

static float Hash2DFloat(int x, int y)
{
    return HashFloat(static_cast<int>(
        static_cast<uint32_t>(x) * 73856093u ^
        static_cast<uint32_t>(y) * 19349663u));
}

// Separate hash for the Y component of cell feature points so X and Y are independent.
static float Hash2DFloatB(int x, int y)
{
    return HashFloat(static_cast<int>(
        static_cast<uint32_t>(x) * 83492791u ^
        static_cast<uint32_t>(y) * 48611u));
}

static int Hash3DInt(int x, int y, int z)
{
    return HashInt(static_cast<int>(
        static_cast<uint32_t>(x) * 73856093u ^
        static_cast<uint32_t>(y) * 19349663u ^
        static_cast<uint32_t>(z) * 83492791u));
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
    const int   xi = FloorToInt(x);
    const int   yi = FloorToInt(y);
    const float xf = Fract(x);
    const float yf = Fract(y);

    const float u = SmootherStep01(xf);
    const float v = SmootherStep01(yf);

    const float n00 = GradientDot2D(Hash2DInt(xi,     yi    ), xf,        yf       );
    const float n10 = GradientDot2D(Hash2DInt(xi + 1, yi    ), xf - 1.0f, yf       );
    const float n01 = GradientDot2D(Hash2DInt(xi,     yi + 1), xf,        yf - 1.0f);
    const float n11 = GradientDot2D(Hash2DInt(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

    const float nx0 = n00 + u * (n10 - n00);
    const float nx1 = n01 + u * (n11 - n01);
    return nx0 + v * (nx1 - nx0);
}

//==============================================================================
// Perlin noise 3D
//==============================================================================

float PerlinNoise(float x, float y, float z)
{
    const int   xi = FloorToInt(x);
    const int   yi = FloorToInt(y);
    const int   zi = FloorToInt(z);
    const float xf = Fract(x);
    const float yf = Fract(y);
    const float zf = Fract(z);

    const float u = SmootherStep01(xf);
    const float v = SmootherStep01(yf);
    const float w = SmootherStep01(zf);

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

    const float nx00 = n000 + u * (n100 - n000);
    const float nx10 = n010 + u * (n110 - n010);
    const float nx01 = n001 + u * (n101 - n001);
    const float nx11 = n011 + u * (n111 - n011);

    const float nxy0 = nx00 + v * (nx10 - nx00);
    const float nxy1 = nx01 + v * (nx11 - nx01);

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
    const float s = (x + y) * F2;
    const int   i = FloorToInt(x + s);
    const int   j = FloorToInt(y + s);

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
    const int   xi = FloorToInt(x);
    const int   yi = FloorToInt(y);
    const float xf = Fract(x);
    const float yf = Fract(y);

    // Compare squared distances to avoid 9 sqrt calls.
    float minDistSq = 4.0f; // (2.0)^2 maximum possible distance across a unit cell

    for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
            const int cx = xi + di;
            const int cy = yi + dj;
            // Independent hashes for X and Y so feature points aren't on the diagonal
            const float fpx = static_cast<float>(di) + Hash2DFloat (cx, cy);
            const float fpy = static_cast<float>(dj) + Hash2DFloatB(cx, cy);

            const float dx = fpx - xf;
            const float dy = fpy - yf;
            const float distSq = dx * dx + dy * dy;
            if (distSq < minDistSq)
                minDistSq = distSq;
        }
    }

    return Clamp(Sqrt(minDistSq), 0.0f, 1.0f);
}

//==============================================================================
// Value noise 2D
//==============================================================================

float ValueNoise(float x, float y)
{
    const int   xi = FloorToInt(x);
    const int   yi = FloorToInt(y);
    const float xf = Fract(x);
    const float yf = Fract(y);

    const float u = SmootherStep01(xf);
    const float v = SmootherStep01(yf);

    const float v00 = Hash2DFloat(xi,     yi    );
    const float v10 = Hash2DFloat(xi + 1, yi    );
    const float v01 = Hash2DFloat(xi,     yi + 1);
    const float v11 = Hash2DFloat(xi + 1, yi + 1);

    const float vx0 = v00 + u * (v10 - v00);
    const float vx1 = v01 + u * (v11 - v01);
    return vx0 + v * (vx1 - vx0);
}

//==============================================================================
// Fractal Brownian Motion
//==============================================================================

float FractalBrownianMotion(float x, float y, int octaves, float persistence, float lacunarity)
{
    if (octaves <= 0) return 0.0f;

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
