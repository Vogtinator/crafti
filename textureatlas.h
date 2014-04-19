#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include "gl.h"

bool loadTextureFromFile(const char* filename, TEXTURE *texture);

struct TextureAtlasEntry
{
    GLFix left, right; //U
    GLFix top, bottom; //V
};

TextureAtlasEntry textureArea(const int x, const int y, const int w, const int h);

#endif // TEXTUREATLAS_H
