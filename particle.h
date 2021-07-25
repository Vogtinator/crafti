#ifndef PARTICLE_H
#define PARTICLE_H

#include "terrain.h"

struct Particle
{
    VECTOR3 pos, vel;
    GLFix size;
    TextureAtlasEntry tae;
    void logic(bool *remove);
    void render();

    // Render always facing the camera and right side up,
    // based on the center coordinates.
    static void render(VECTOR3 center, GLFix size, const TextureAtlasEntry &tae);
};

#endif // PARTICLE_H
