#ifndef AABB_H
#define AABB_H

#include "gl.h"

class AABB
{
public:
    enum SIDE
    {
        FRONT=0,
        BACK,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        NONE
    };

    AABB() : AABB(0, 0, 0, 0, 0, 0) {}
    AABB(VERTEX *v1, VERTEX *v2);
    AABB(GLFix low_x, GLFix low_y, GLFix low_z, GLFix high_x, GLFix high_y, GLFix high_z);
    AABB(VERTEX *list, unsigned int size);

    void set(VERTEX *v1, VERTEX *v2);
    void set(GLFix low_x, GLFix low_y, GLFix low_z, GLFix high_x, GLFix high_y, GLFix high_z);
    void set(VERTEX *list, unsigned int size);

    bool intersects(AABB &other);
    SIDE intersectsRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, GLFix &dist);

    void render();
    void print();

    GLFix low_x, low_y, low_z, high_x, high_y, high_z;
};

#endif // AABB_H
