#ifndef TEXTURETOOLS_H
#define TEXTURETOOLS_H

#include "gl.h"

TEXTURE* newTexture(unsigned int w, unsigned int h);
void deleteTexture(TEXTURE *tex);

//Textures have to have the same resolution
void copyTexture(TEXTURE &src, TEXTURE &dest);

//Returns nullptr if loading failed
TEXTURE *loadTextureFromFile(const char* filename);
void drawTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);
//50% opacity
void drawTextureOverlay(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);
//Black parts not drawn
void drawTransparentTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h);
//Allocates memory for new texture, deleteTexture must be called
TEXTURE* resizeTexture(TEXTURE &src, unsigned int w, unsigned int h);

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
