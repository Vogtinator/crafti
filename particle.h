#ifndef PARTICLE_H
#define PARTICLE_H

#include "terrain.h"

// A textured particle with position, size and velocity. Affected by gravity.
// It's rendered around the center position on the screen as quad.
struct Particle
{
    VECTOR3 pos, vel;
    GLFix size;
    TextureAtlasEntry tae;

    void logic(bool *remove);
    void render();
    static void render(VECTOR3 center, GLFix size, const TextureAtlasEntry &tae);
};

#endif // PARTICLE_H
