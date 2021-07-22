#include "particle.h"

void Particle::logic(bool *remove)
{
    size -= 1; // Become smaller
    pos.x += vel.x; pos.y += vel.y; pos.z += vel.z; // Apply velocity to position
    vel.y -= 1; // Gravity

    if(size <= GLFix(5))
        *remove = true;
}

void Particle::render()
{
    glBindTexture(terrain_current);
    render(pos, size, tae);
}

void Particle::render(VECTOR3 center, GLFix size, const TextureAtlasEntry &tae)
{
    nglMultMatVectRes(transformation, &center, &center);
    VERTEX v1{center.x - size/2, center.y - size/2, center.z, tae.left, tae.bottom,
              TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE},
           v2{center.x - size/2, center.y + size/2, center.z, tae.left, tae.top,
              TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE},
           v3{center.x + size/2, center.y + size/2, center.z, tae.right, tae.top,
              TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE},
           v4{center.x + size/2, center.y - size/2, center.z, tae.right, tae.bottom,
              TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE};

    nglDrawTriangle(&v1, &v2, &v3, false);
    nglDrawTriangle(&v3, &v4, &v1, false);
}
