#include "glbox.h"

GLBox::GLBox(GLFix w, GLFix h, GLFix l, GLFix cenX, GLFix cenY, GLFix cenZ,
             TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bo, TextureAtlasEntry le, TextureAtlasEntry ri,
             bool draw_front, bool draw_back, bool draw_top, bool draw_bottom, bool draw_left, bool draw_right)
    : cenX{cenX}, cenY{cenY}, cenZ{cenZ}
{
    if(draw_front)
    {
        vertices.push_back({0, 0, 0, fr.left, fr.bottom, 0});
        vertices.push_back({0, h, 0, fr.left, fr.top, 0});
        vertices.push_back({w, h, 0, fr.right, fr.top, 0});
        vertices.push_back({w, h, 0, fr.right, fr.top, 0});
        vertices.push_back({w, 0, 0, fr.right, fr.bottom, 0});
        vertices.push_back({0, 0, 0, fr.left, fr.bottom, 0});
    }

    if(draw_back)
    {
        vertices.push_back({w, 0, l, ba.left, ba.bottom, 0});
        vertices.push_back({w, h, l, ba.left, ba.top, 0});
        vertices.push_back({0, h, l, ba.right, ba.top, 0});
        vertices.push_back({0, h, l, ba.right, ba.top, 0});
        vertices.push_back({0, 0, l, ba.right, ba.bottom, 0});
        vertices.push_back({w, 0, l, ba.left, ba.bottom, 0});
    }

    if(draw_left)
    {
        vertices.push_back({0, 0, l, le.left, le.bottom, 0});
        vertices.push_back({0, h, l, le.left, le.top, 0});
        vertices.push_back({0, h, 0, le.right, le.top, 0});
        vertices.push_back({0, h, 0, le.right, le.top, 0});
        vertices.push_back({0, 0, 0, le.right, le.bottom, 0});
        vertices.push_back({0, 0, l, le.left, le.bottom, 0});
    }

    if(draw_right)
    {
        vertices.push_back({w, 0, 0, ri.left, ri.bottom, 0});
        vertices.push_back({w, h, 0, ri.left, ri.top, 0});
        vertices.push_back({w, h, l, ri.right, ri.top, 0});
        vertices.push_back({w, h, l, ri.right, ri.top, 0});
        vertices.push_back({w, 0, l, ri.right, ri.bottom, 0});
        vertices.push_back({w, 0, 0, ri.left, ri.bottom, 0});
    }

    if(draw_top)
    {
        vertices.push_back({0, h, 0, to.left, to.bottom, 0});
        vertices.push_back({0, h, l, to.left, to.top, 0});
        vertices.push_back({w, h, l, to.right, to.top, 0});
        vertices.push_back({w, h, l, to.right, to.top, 0});
        vertices.push_back({w, h, 0, to.right, to.bottom, 0});
        vertices.push_back({0, h, 0, to.left, to.bottom, 0});
    }

    if(draw_bottom)
    {
        vertices.push_back({w, 0, 0, bo.left, bo.bottom, 0});
        vertices.push_back({w, 0, l, bo.left, bo.top, 0});
        vertices.push_back({0, 0, l, bo.right, bo.top, 0});
        vertices.push_back({0, 0, l, bo.right, bo.top, 0});
        vertices.push_back({0, 0, 0, bo.right, bo.bottom, 0});
        vertices.push_back({w, 0, 0, bo.left, bo.bottom, 0});
    }
}

void GLBox::draw()
{
    glPushMatrix();

    glTranslatef(posX, posY, posZ);
    nglRotateY(rotY.normaliseAngle());
    nglRotateX(rotX.normaliseAngle());
    nglRotateZ(rotZ.normaliseAngle());
    glTranslatef(-cenX, -cenY, -cenZ);

    glBegin(GL_TRIANGLES);
    nglAddVertices(vertices.data(), vertices.size());
    glEnd();

    //Childs aligned on cen{X,Y,Z}
    glTranslatef(cenX, cenY, cenZ);

    for(auto i : children)
        i->draw();

    glPopMatrix();
}
