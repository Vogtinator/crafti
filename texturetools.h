#ifndef TEXTURETOOLS_H
#define TEXTURETOOLS_H

#include "gl.h"

bool loadTextureFromFile(const char* filename, TEXTURE *texture);
void drawTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);
void drawTextureOverlay(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);
void drawTransparentTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);

//Show loading text above, will be cleared on the next rendered frame, doesn't really belong here
//Increases internal counter, if it's >= i it's shown, if i == -1, reset the internal counter
void drawLoadingtext(int i);

struct TextureAtlasEntry
{
    GLFix left, right; //U
    GLFix top, bottom; //V
};

TextureAtlasEntry textureArea(const int x, const int y, const int w, const int h);

#endif // TEXTURETOOLS_H
