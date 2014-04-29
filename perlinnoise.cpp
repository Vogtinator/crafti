#include "perlinnoise.h"

#include <cmath>
#include <random>
#include <algorithm>

// THIS IS A DIRECT TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

PerlinNoise::PerlinNoise(unsigned int seed)
{
    setSeed(seed);
}

// Generate a new permutation vector based on the value of seed
void PerlinNoise::setSeed(unsigned int seed)
{
    p.clear();
    p.resize(256);

    // Fill p with values from 0 to 255
    std::iota(p.begin(), p.end(), 0);

    // Initialize a random engine with seed
    std::default_random_engine engine(seed);

    // Suffle using the above random engine
    std::shuffle(p.begin(), p.end(), engine);

    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
}

GLFix PerlinNoise::noise(GLFix x, GLFix y, GLFix z) const {
    // Find the unit cube that contains the point
    int X = x.floor() & 255;
    int Y = y.floor() & 255;
    int Z = z.floor() & 255;

    // Find relative x, y,z of point in cube
    x -= x.floor();
    y -= y.floor();
    z -= z.floor();

    // Compute fade curves for each of x, y, z
    GLFix u = fade(x);
    GLFix v = fade(y);
    GLFix w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    GLFix res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x-1, y, z)), lerp(u, grad(p[AB], x, y-1, z), grad(p[BB], x-1, y-1, z))),	lerp(v, lerp(u, grad(p[AA+1], x, y, z-1), grad(p[BA+1], x-1, y, z-1)), lerp(u, grad(p[AB+1], x, y-1, z-1),	grad(p[BB+1], x-1, y-1, z-1))));
    return (res + 1) >> 1;
}

GLFix PerlinNoise::fade(GLFix t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

GLFix PerlinNoise::lerp(GLFix t, GLFix a, GLFix b) const {
    return a + t * (b - a);
}

GLFix PerlinNoise::grad(int hash, GLFix x, GLFix y, GLFix z) const {
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    GLFix u = h < 8 ? x : y,
           v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
