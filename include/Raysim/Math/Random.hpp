#pragma once

#include <random>

namespace RS {

/**
 * @namespace Math::Random
 * @brief Pseudo-random number generation and noise utilities
 *
 * Provides both traditional random number generation and procedural noise functions.
 *
 * Random generation uses std::mt19937 (Mersenne Twister) for high-quality randomness.
 * Noise functions (Perlin, Simplex, Cellular) are useful for procedural generation,
 * terrain creation, and natural-looking animations.
 *
 * Examples:
 * ```
 * auto& random = Math::Random;
 *
 * // Random integers and floats
 * int dice = random::Int(100);
 * float normalized = random::Float();
 * float velocity = random::Range(-10.0f, 10.0f);
 *
 * // Procedural noise
 * float elevation = random::PerlinNoise(x, y);           // Smooth 2D noise (-1 to 1)
 * float terrain = random::SimplexNoise(x, y) * 100.0f;   // Scaled Simplex noise
 * float cells = random::CellularNoise(x, y);             // Worley/cell noise for patterns
 * ```
 */
namespace Random {
    /// @brief Seed the random number generator
    /// @param seed Seed value (use time-based seed for unpredictable randomness)
    void Seed(unsigned int seed);

    /// @brief Random integer in range [0, max)
    /// @param max Upper bound (exclusive)
    /// @return Random integer
    int Int(int max);

    /// @brief Random integer in range [min, max)
    /// @param min Lower bound (inclusive)
    /// @param max Upper bound (exclusive)
    /// @return Random integer in range
    int Range(int min, int max);

    /// @brief Random float in range [0.0, 1.0)
    /// @return Random normalized float
    float Float();

    /// @brief Random float in range [0.0, max)
    /// @param max Upper bound
    /// @return Random float
    float Float(float max);

    /// @brief Random float in range [min, max)
    /// @param min Lower bound
    /// @param max Upper bound
    /// @return Random float in range
    float Range(float min, float max);

    /// @brief Random choice from container (vector, array, etc.)
    /// @tparam T Container type (must have size() and operator[])
    /// @param container Container to choose from
    /// @return Random element from container
    /// @note Undefined behavior if container is empty
    template<typename T>
    typename T::value_type Choice(const T& container) {
        if (container.empty()) return typename T::value_type{};
        return container[Int(static_cast<int>(container.size()))];
    }

    /// @brief Random boolean with 50% probability
    /// @return Random bool
    bool Bool();

    /// @brief Random boolean with custom probability
    /// @param probability Probability of true (0.0-1.0)
    /// @return Random bool
    bool Bool(float probability);

    // ============================================================================
    // NOISE GENERATION (Procedural)
    // ============================================================================

    /**
     * @brief Perlin noise in 2D
     *
     * Generates smooth, continuous noise useful for terrain, clouds, and animations.
     * Returns values in range approximately [-1.0, 1.0].
     *
     * @param x X coordinate in noise space
     * @param y Y coordinate in noise space
     * @return Perlin noise value at (x, y)
     * @note Same input always produces same output (deterministic)
     * @note Octave multiple calls with different scales for fractal Brownian motion:
     * ```
     * float fbm = 0.0f, amplitude = 1.0f;
     * for (int i = 0; i < octaves; i++) {
     *     fbm += PerlinNoise(x * scale, y * scale) * amplitude;
     *     scale *= 2.0f;
     *     amplitude *= 0.5f;
     * }
     * ```
     * @example
     * // Smooth terrain height variation
     * float height = PerlinNoise(worldX * 0.1f, worldY * 0.1f);
     */
    float PerlinNoise(float x, float y);

    /**
     * @brief Perlin noise in 3D
     *
     * 3D variant for volumetric effects, 3D terrain, or animation over time.
     *
     * @param x X coordinate in noise space
     * @param y Y coordinate in noise space
     * @param z Z coordinate in noise space (often time for animation)
     * @return Perlin noise value at (x, y, z)
     * @example
     * // Animated cloud effect with time as z
     * float cloudDensity = PerlinNoise(screenX * 0.01f, screenY * 0.01f, time);
     */
    float PerlinNoise(float x, float y, float z);

    /**
     * @brief Simplex noise in 2D
     *
     * Improved alternative to Perlin noise (faster, fewer artifacts).
     * Returns values in range approximately [-1.0, 1.0].
     *
     * @param x X coordinate in noise space
     * @param y Y coordinate in noise space
     * @return Simplex noise value
     * @see PerlinNoise() for differences
     * @example
     * // Better-looking procedural terrain than Perlin
     * float terrain = SimplexNoise(x * 0.05f, y * 0.05f);
     */
    float SimplexNoise(float x, float y);

    /**
     * @brief Cellular (Worley) noise in 2D
     *
     * Creates organic patterns like cells, cracks, or scales.
     * Returns values in range [0.0, 1.0].
     *
     * @param x X coordinate in noise space
     * @param y Y coordinate in noise space
     * @return Distance-based cellular noise
     * @note Returns distance to nearest feature point
     * @example
     * // Organic cracking patterns
     * float cracks = 1.0f - CellularNoise(x * 0.02f, y * 0.02f);
     *
     * // Cellular creature pattern
     * if (CellularNoise(x * 0.1f, y * 0.1f) > 0.5f) {
     *     // Interior of cell
     * }
     */
    float CellularNoise(float x, float y);

    /**
     * @brief Value noise in 2D
     *
     * Simple interpolated noise (faster but less smooth than Perlin).
     * Returns values in range [0.0, 1.0].
     *
     * @param x X coordinate in noise space
     * @param y Y coordinate in noise space
     * @return Value noise at (x, y)
     * @example
     * // Fast procedural texture
     * float texture = ValueNoise(texU * 4.0f, texV * 4.0f);
     */
    float ValueNoise(float x, float y);

    /**
     * @brief Fractal Brownian Motion (fBm)
     *
     * Combines multiple octaves of noise for natural-looking variation.
     * Commonly used for terrain, clouds, and natural textures.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param octaves Number of noise layers (typically 4-8)
     * @param persistence Amplitude reduction per octave (0-1, typically 0.5)
     * @param scale Frequency multiplier per octave (typically 2.0)
     * @return Fractal noise value
     * @example
     * // Realistic mountainous terrain
     * float mountain = FractalBrownianMotion(x * 0.01f, y * 0.01f,
     *                                         6, 0.5f, 2.0f) * 200.0f;
     */
    float FractalBrownianMotion(float x, float y, int octaves, float persistence, float scale);

} // namespace Random

} // namespace RS
