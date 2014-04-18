#ifndef GLBOX_H
#define GLBOX_H

#include <memory>
#include <vector>

#include "gldrawable.h"
#include "textureatlas.h"

class GLBox : public GLDrawable
{
public:
    GLBox(GLFix width, GLFix height, GLFix length, GLFix cenX, GLFix cenY, GLFix cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri, bool draw_front, bool draw_back, bool draw_top, bool draw_bottom, bool draw_left, bool draw_right);
    GLBox(float width, float height, float length, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri)
        : GLBox(width, height, length, cenX, cenY, cenZ, fr, ba, to, bot, le, ri, true, true, true, true, true, true) {}

    void addChild(std::shared_ptr<GLDrawable> child) { children.push_back(child); }
    void draw();

private:
    GLFix cenX, cenY, cenZ; //Center of rotation

    std::vector<VERTEX> vertices;
    std::vector<std::shared_ptr<GLDrawable> > children;
};

#endif // GLBOX_H
