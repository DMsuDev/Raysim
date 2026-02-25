#include "Raysim/Math/Random.hpp"

#include <ctime>
#include <cmath>
#include <algorithm>

namespace RS::Random {

static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));

void Seed(unsigned int seed) {
    generator.seed(seed);
}

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
    probability = (probability < 0.0f) ? 0.0f : (probability > 1.0f) ? 1.0f : probability;
    return Float() < probability;
}

// ============================================================================
// NOISE GENERATION
// ============================================================================

// Hash function for pseudo-random gradient selection
static float Hash(int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 0x27d4eb2d;
    seed ^= seed >> 15;
    return (static_cast<float>(seed) / 2147483648.0f + 1.0f) * 0.5f;
}

static float Hash2D(int x, int y) {
    return Hash((x * 73856093) ^ (y * 19349663));
}

static float Hash3D(int x, int y, int z) {
    return Hash((x * 73856093) ^ (y * 19349663) ^ (z * 83492791));
}

// Smooth interpolation function (fade curve)
static float SmoothStep(float t) {
    return t * t * (3.0f - 2.0f * t);
}

// Catmull-Rom interpolation
static float CatmullRom(float p0, float p1, float p2, float p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        2.0f * p1 +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}

// Gradient vector dot product for Perlin noise
static float GradientDot(int hash, float x, float y) {
    int h = hash & 15;
    float u = (h < 8) ? x : y;
    float v = (h < 8) ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float PerlinNoise(float x, float y) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;

    float u = SmoothStep(xf);
    float v = SmoothStep(yf);

    float n00 = GradientDot(Hash2D(xi, yi), xf, yf);
    float n10 = GradientDot(Hash2D(xi + 1, yi), xf - 1.0f, yf);
    float n01 = GradientDot(Hash2D(xi, yi + 1), xf, yf - 1.0f);
    float n11 = GradientDot(Hash2D(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

    float nx0 = n00 * (1.0f - u) + n10 * u;
    float nx1 = n01 * (1.0f - u) + n11 * u;
    return nx0 * (1.0f - v) + nx1 * v;
}

float PerlinNoise(float x, float y, float z) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    int zi = static_cast<int>(std::floor(z));
    float xf = x - xi;
    float yf = y - yi;
    float zf = z - zi;

    float u = SmoothStep(xf);
    float v = SmoothStep(yf);
    float w = SmoothStep(zf);

    auto grad3d = [](int hash, float x, float y, float z) {
        int h = hash & 15;
        float u = (h < 8) ? x : y;
        float v = (h < 4) ? y : (h == 12 || h == 14) ? x : z;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    };

    float n000 = grad3d(Hash3D(xi, yi, zi), xf, yf, zf);
    float n100 = grad3d(Hash3D(xi + 1, yi, zi), xf - 1.0f, yf, zf);
    float n010 = grad3d(Hash3D(xi, yi + 1, zi), xf, yf - 1.0f, zf);
    float n110 = grad3d(Hash3D(xi + 1, yi + 1, zi), xf - 1.0f, yf - 1.0f, zf);
    float n001 = grad3d(Hash3D(xi, yi, zi + 1), xf, yf, zf - 1.0f);
    float n101 = grad3d(Hash3D(xi + 1, yi, zi + 1), xf - 1.0f, yf, zf - 1.0f);
    float n011 = grad3d(Hash3D(xi, yi + 1, zi + 1), xf, yf - 1.0f, zf - 1.0f);
    float n111 = grad3d(Hash3D(xi + 1, yi + 1, zi + 1), xf - 1.0f, yf - 1.0f, zf - 1.0f);

    float nx00 = n000 * (1.0f - u) + n100 * u;
    float nx10 = n010 * (1.0f - u) + n110 * u;
    float nx01 = n001 * (1.0f - u) + n101 * u;
    float nx11 = n011 * (1.0f - u) + n111 * u;

    float nxy0 = nx00 * (1.0f - v) + nx10 * v;
    float nxy1 = nx01 * (1.0f - v) + nx11 * v;

    return nxy0 * (1.0f - w) + nxy1 * w;
}

float SimplexNoise(float x, float y) {
    // Simplex noise (simplified 2D version)
    // Similar to Perlin but with fewer gradient lookups
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;

    // Determine skew
    int i1 = (xf > yf) ? 1 : 0;
    int j1 = (xf > yf) ? 0 : 1;

    float t = (xf + yf) * 0.5f;
    float u = SmoothStep(xf);
    float v = SmoothStep(yf);

    float n0 = GradientDot(Hash2D(xi, yi), xf, yf);
    float n1 = GradientDot(Hash2D(xi + i1, yi + j1), xf - i1, yf - j1);
    float n2 = GradientDot(Hash2D(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

    float result = n0 * (1.0f - u - v) + n1 * (u + v - 1.0f) + n2 * (2.0f * u * v);
    return result * 0.5f;
}

float CellularNoise(float x, float y) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;

    float minDist = 2.0f;

    // Check nearby cells for feature points
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            // Generate pseudo-random feature point in this cell
            float cellX = Hash2D(xi + i, yi + j) * 0.9f + i;
            float cellY = Hash2D(xi + i, yi + j) * 0.9f + j;

            float dx = cellX - xf;
            float dy = cellY - yf;
            float dist = std::sqrt(dx * dx + dy * dy);

            minDist = std::min(minDist, dist);
        }
    }

    return std::max(0.0f, std::min(1.0f, minDist));
}

float ValueNoise(float x, float y) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;

    float u = SmoothStep(xf);
    float v = SmoothStep(yf);

    float v00 = Hash2D(xi, yi);
    float v10 = Hash2D(xi + 1, yi);
    float v01 = Hash2D(xi, yi + 1);
    float v11 = Hash2D(xi + 1, yi + 1);

    float vx0 = v00 * (1.0f - u) + v10 * u;
    float vx1 = v01 * (1.0f - u) + v11 * u;

    return vx0 * (1.0f - v) + vx1 * v;
}

float FractalBrownianMotion(float x, float y, int octaves, float persistence, float scale) {
    float result = 0.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    float freq = 1.0f;

    for (int i = 0; i < octaves; ++i) {
        result += PerlinNoise(x * freq, y * freq) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;
        freq *= scale;
    }

    return result / maxValue;
}

} // namespace RS::Random
