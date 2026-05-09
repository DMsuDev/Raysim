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

namespace RS::Math {

/**
 * @namespace Math::Random
 * @brief Pseudo-random number generation and coherent noise utilities.
 *
 * This namespace provides two families of functions:
 *
 * 1. Standard RNG - Uniform random numbers (integers, floats, booleans)
 *    backed by a Mersenne Twister (std::mt19937) engine.
 *
 * 2. Procedural noise - Deterministic, spatially-coherent noise functions
 *    (Perlin, Simplex, Cellular, Value) plus Fractal Brownian Motion for
 *    layering multiple octaves.
 *
 * Both families share a common seed.  On startup the seed is initialised from
 * the hardware random device (std::random_device), so every launch produces
 * different results by default.  Call Seed() with a fixed value for
 * reproducible output, or SeedRandom() to re-randomise at any time.
 *
 * @note Noise functions are not thread-safe because they read a shared seed.
 */
namespace Random {

    // ========================================================================
    // Seeding
    // ========================================================================

    /**
     * @brief Set a deterministic seed for both the RNG and all noise functions.
     *
     * After calling this, every subsequent call to any function in this
     * namespace will produce the same sequence of values for the same seed.
     * Useful for reproducible simulations or debugging.
     *
     * @param seed  The seed value to use.
     */
    void Seed(unsigned int seed);

    /**
     * @brief Re-seed with a hardware-random value (non-deterministic).
     *
     * Replaces the current seed with a fresh value obtained from
     * std::random_device.  Call this to get a new random landscape or
     * sequence without restarting the application.
     */
    void SeedRandom();

    /**
     * @brief Return the current noise seed.
     *
     * The noise seed is the value that was last set by Seed() or SeedRandom()
     * (or the initial hardware-random value if neither was called).
     * Useful for displaying the seed on-screen or serialising it so the same
     * landscape can be reconstructed later.
     *
     * @return The active noise seed.
     */
    unsigned int GetNoiseSeed();

    // ========================================================================
    // Standard RNG
    // ========================================================================

    /**
     * @brief Generate a random integer in the range [0, max).
     *
     * The result is uniformly distributed.  If max <= 0 the behaviour is
     * undefined.
     *
     * @param max  Upper bound (exclusive).
     * @return     A random integer in [0, max).
     */
    int Int(int max);

    /**
     * @brief Generate a random integer in the range [min, max).
     *
     * The result is uniformly distributed.  min must be less than max.
     *
     * @param min  Lower bound (inclusive).
     * @param max  Upper bound (exclusive).
     * @return     A random integer in [min, max).
     */
    int Range(int min, int max);

    /**
     * @brief Generate a random float in the range [0.0, 1.0).
     * @return A uniformly distributed float in [0.0, 1.0).
     */
    float Float();

    /**
     * @brief Generate a random float in the range [0.0, max).
     *
     * Equivalent to Float() * max but correctly handles edge cases at the
     * upper bound.
     *
     * @param max  Upper bound (exclusive).
     * @return     A random float in [0.0, max).
     */
    float Float(float max);

    /**
     * @brief Generate a random float in the range [min, max).
     *
     * @param min  Lower bound (inclusive).
     * @param max  Upper bound (exclusive).
     * @return     A uniformly distributed float in [min, max).
     */
    float Range(float min, float max);

    /**
     * @brief Pick a random element from a container.
     *
     * Works with any container that exposes size() and operator[] (e.g.
     * std::vector, std::array, std::string).  Returns a default-constructed
     * value if the container is empty.
     *
     * @tparam T          Container type.
     * @param  container  The container to sample from.
     * @return            A randomly chosen element, or T::value_type{} if empty.
     */
    template<typename T>
    typename T::value_type Choice(const T& container) {
        if (container.empty()) return typename T::value_type{};
        return container[Int(static_cast<int>(container.size()))];
    }

    /**
     * @brief Generate a random boolean with 50/50 probability.
     * @return true or false, each with 50% chance.
     */
    bool Bool();

    /**
     * @brief Generate a random boolean with a custom probability of being true.
     *
     * @param probability  Chance of returning true, in the range [0.0, 1.0].
     *                     0.0 always returns false, 1.0 always returns true.
     * @return             true with the given probability, false otherwise.
     */
    bool Bool(float probability);

    // ========================================================================
    // Noise Generation (procedural, deterministic, affected by Seed)
    // ========================================================================

    /**
     * @brief 2D Perlin noise (gradient noise).
     *
     * Returns a smooth, continuous value based on the input coordinates.
     * The output range is approximately [-1, 1].  Perlin noise is useful for
     * terrain heightmaps, cloud textures, and organic-looking variation.
     *
     * @param x  X coordinate in noise space.
     * @param y  Y coordinate in noise space.
     * @return   Noise value in approximately [-1, 1].
     */
    float PerlinNoise(float x, float y);

    /**
     * @brief 3D Perlin noise (gradient noise).
     *
     * Same as the 2D version but with an extra axis.  A common use is to pass
     * time as the Z coordinate to create smoothly animated noise fields.
     *
     * @param x  X coordinate in noise space.
     * @param y  Y coordinate in noise space.
     * @param z  Z coordinate in noise space (can be used as time).
     * @return   Noise value in approximately [-1, 1].
     */
    float PerlinNoise(float x, float y, float z);

    /**
     * @brief 2D Simplex noise.
     *
     * Similar to Perlin noise but evaluated on a triangular (simplex) lattice
     * instead of a square grid, which reduces directional artifacts and is
     * slightly faster in higher dimensions.  Output range is approximately
     * [-1, 1].
     *
     * @param x  X coordinate in noise space.
     * @param y  Y coordinate in noise space.
     * @return   Noise value in approximately [-1, 1].
     */
    float SimplexNoise(float x, float y);

    /**
     * @brief 2D Cellular (Worley) noise.
     *
     * Computes the distance to the nearest randomly placed feature point in
     * each grid cell.  Produces patterns that resemble biological cell
     * structures, stone walls, or water caustics.  The output is normalised
     * to the range [0, 1].
     *
     * @param x  X coordinate in noise space.
     * @param y  Y coordinate in noise space.
     * @return   Normalised distance value in [0, 1].
     */
    float CellularNoise(float x, float y);

    /**
     * @brief 2D Value noise.
     *
     * Assigns a random value to each integer lattice point and interpolates
     * smoothly between them using a quintic curve.  Faster than Perlin but
     * can show more grid-aligned artifacts.  Output range is [0, 1].
     *
     * @param x  X coordinate in noise space.
     * @param y  Y coordinate in noise space.
     * @return   Noise value in [0, 1].
     */
    float ValueNoise(float x, float y);

    /**
     * @brief Fractal Brownian Motion (fBm) using layered Perlin noise.
     *
     * Sums multiple octaves of Perlin noise at increasing frequencies and
     * decreasing amplitudes.  This produces self-similar detail at many
     * scales, ideal for terrain, clouds, and other natural phenomena.
     *
     * @param x            X coordinate in noise space.
     * @param y            Y coordinate in noise space.
     * @param octaves      Number of noise layers to sum (4-8 is typical).
     * @param persistence  Amplitude multiplier per octave (0.0-1.0, typically 0.5).
     *                     Lower values produce smoother results.
     * @param lacunarity   Frequency multiplier per octave (typically 2.0).
     *                     Higher values add finer detail sooner.
     * @return             Combined noise value (range depends on octaves and persistence).
     */
    float FractalBrownianMotion(float x, float y, int octaves = 6,
                                float persistence = 0.5f, float lacunarity = 2.0f);

} // namespace Random

} // namespace RS::Math
