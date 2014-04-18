#ifndef GLDRAWABLE_H
#define GLDRAWABLE_H

#include "gl.h"

class GLDrawable
{
public:
    virtual ~GLDrawable() {}

    void setXPosition(GLFix x) { posX = x; }
    GLFix getXPosition() const { return posX; }
    void setYPosition(GLFix y) { posY = y; }
    GLFix getYPosition() const { return posY; }
    void setZPosition(GLFix z) { posZ = z; }
    GLFix getZPosition() const { return posZ; }
    void setXRotation(GLFix x) { rotX = x; }
    GLFix getXRotation() const { return rotX; }
    void setYRotation(GLFix y) { rotY = y; }
    GLFix getYRotation() const { return rotY; }
    void setZRotation(GLFix z) { rotZ = z; }
    GLFix getZRotation() const { return rotZ; }

    virtual void draw() = 0;

protected:
    GLFix posX = 0, posY = 0, posZ = 0;
    GLFix rotX = 0, rotY = 0, rotZ = 0;
};

#endif // GLDRAWABLE_H
