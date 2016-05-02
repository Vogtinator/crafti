#include "aabb.h"

#include <algorithm>

AABB::AABB(VERTEX *v1, VERTEX *v2)
{
    set(v1, v2);
}

AABB::AABB(GLFix low_x, GLFix low_y, GLFix low_z, GLFix high_x, GLFix high_y, GLFix high_z)
    : low_x(low_x), low_y(low_y), low_z(low_z), high_x(high_x), high_y(high_y), high_z(high_z)
{}

AABB::AABB(VERTEX *list, unsigned int size)
{
    set(list, size);
}

void AABB::set(VERTEX *v1, VERTEX *v2)
{
    low_x = std::min(v1->x, v1->y);
    high_x = std::max(v1->x, v2->x);
    low_y = std::min(v1->y, v2->y);
    high_y = std::max(v1->y, v2->y);
    low_z = std::min(v1->z, v2->z);
    high_z = std::max(v1->z, v2->z);
}

void AABB::set(GLFix low_x, GLFix low_y, GLFix low_z, GLFix high_x, GLFix high_y, GLFix high_z)
{
    this->low_x = low_x;
    this->low_y = low_y;
    this->low_z = low_z;
    this->high_x = high_x;
    this->high_y = high_y;
    this->high_z = high_z;
}

void AABB::set(VERTEX *list, unsigned int size)
{
    low_x = high_x = list->x;
    low_y = high_y = list->y;
    low_z = high_z = list->z;
    list++;

    for(unsigned int i = 1; i < size; i++)
    {
        VERTEX *v = list++;

        if(v->x < low_x)
            low_x = v->x;
        if(v->x > high_x)
            high_x = v->x;
        if(v->y < low_y)
            low_y = v->y;
        if(v->y > high_y)
            high_y = v->y;
        if(v->z < low_z)
            low_z = v->z;
        if(v->z > high_z)
            high_z = v->z;
    }
}

bool AABB::intersects(AABB &other)
{
    return high_x >= other.low_x && high_y >= other.low_y && high_z >= other.low_z
            && low_x <= other.high_x && low_y <= other.high_y && low_z <= other.high_z;
}

AABB::SIDE AABB::intersectsRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, GLFix &dist)
{
    GLFix t_min_x = 0, t_max_x = GLFix::maxValue(),
            t_min_y = 0, t_max_y = GLFix::maxValue(),
            t_min_z = 0, t_max_z = GLFix::maxValue();

    if(dx == GLFix(0))
    {
        if(x < low_x || x > high_x)
            return NONE;
    }
    else
    {
        t_min_x = (low_x - x) / dx;
        t_max_x = (high_x - x) / dx;
    }

    if(dy == GLFix(0))
    {
        if(y < low_y || y > high_y)
            return NONE;
    }
    else
    {
        t_min_y = (low_y - y) / dy;
        t_max_y = (high_y - y) / dy;
    }

    if(dz == GLFix(0))
    {
        if(z < low_z || z > high_z)
            return NONE;
    }
    else
    {
        t_min_z = (low_z - z) / dz;
        t_max_z = (high_z - z) / dz;
    }

    GLFix min = std::max(std::max(std::min(t_min_x, t_max_x), std::min(t_min_y, t_max_y)), std::min(t_min_z, t_max_z));
    GLFix max = std::min(std::min(std::max(t_min_x, t_max_x), std::max(t_min_y, t_max_y)), std::max(t_min_z, t_max_z));

    if(max < GLFix(0) || min > max)
        return NONE;

    dist = min;

    if(min == t_min_x || min == t_max_x)
        return dx < GLFix(0) ? RIGHT : LEFT;

    if(min == t_min_y || min == t_max_y)
        return dy < GLFix(0) ? TOP : BOTTOM;

    //if(min == t_min_z || min == t_max_z)
        return dz < GLFix(0) ? BACK : FRONT;
}

void AABB::render()
{
    VERTEX v1{low_x, low_y, low_z, 0, 0, 0},
            v2{high_x, low_y, low_z, 0, 0, 0},
            v3{low_x, high_y, low_z, 0, 0, 0},
            v4{high_x, high_y, low_z, 0, 0, 0},
            v5{low_x, low_y, high_z, 0, 0, 0},
            v6{high_x, low_y, high_z, 0, 0, 0},
            v7{low_x, high_y, high_z, 0, 0, 0},
            v8{high_x, high_y, high_z, 0, 0, 0};

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v1);
        nglAddVertex(&v2);
        nglAddVertex(&v4);
        nglAddVertex(&v3);
        nglAddVertex(&v1);
    glEnd();

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v5);
        nglAddVertex(&v6);
        nglAddVertex(&v8);
        nglAddVertex(&v7);
        nglAddVertex(&v5);
    glEnd();

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v1);
        nglAddVertex(&v5);
    glEnd();

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v2);
        nglAddVertex(&v6);
    glEnd();

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v3);
        nglAddVertex(&v7);
    glEnd();

    glBegin(GL_LINE_STRIP);
        nglAddVertex(&v4);
        nglAddVertex(&v8);
    glEnd();
}

void AABB::print()
{
    printf("X: %d-%d Y: %d-%d Z: %d-%d", low_x.toInteger<int>(), high_x.toInteger<int>(), low_y.toInteger<int>(), high_y.toInteger<int>(), low_z.toInteger<int>(), high_z.toInteger<int>());
}
