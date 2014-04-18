#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>

#include "gl.h"

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

// A NOTE TO THE ORIGINAL AUTHOR OF THIS VERSION: WHY ARE YOU SHOUTING?????

class PerlinNoise {
    // The permutation vector
    std::vector<int> p;
public:
    // Initialize with the reference values for the permutation vector
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    // Generate a new permutation vector based on the value of seed
    void setSeed(unsigned int seed);
    // Get a noise value, for 2D images z can have any value
    float noise(GLFix x, GLFix y, GLFix z) const;
private:
    GLFix fade(GLFix t) const;
    GLFix lerp(GLFix t, GLFix a, GLFix b) const;
    GLFix grad(int hash, GLFix x, GLFix y, GLFix z) const;
};

#endif
