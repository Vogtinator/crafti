#ifndef TEXTURETOOLS_H
#define TEXTURETOOLS_H

#include "gl.h"

TEXTURE* newTexture(unsigned int w, unsigned int h);
void deleteTexture(TEXTURE *tex);

//Textures have to have the same resolution
void copyTexture(const TEXTURE &src, TEXTURE &dest);

//Returns nullptr if loading failed
TEXTURE *loadTextureFromFile(const char* filename);
void drawTexture(const TEXTURE &src, const int src_x, const int src_y, TEXTURE &dest, const int dest_x, const int dest_y, const int w, const int h);
//50% opacity
void drawTextureOverlay(const TEXTURE &src, const int src_x, const int src_y, TEXTURE &dest, const int dest_x, const int dest_y, const int w, const int h);
//Black parts not drawn
void drawTransparentTexture(const TEXTURE &src, const int src_x, const int src_y, TEXTURE &dest, const int dest_x, const int dest_y, const int w, const int h);
//Allocates memory for new texture, deleteTexture must be called
TEXTURE* resizeTexture(const TEXTURE &src, const unsigned int w, const unsigned int h);
//Makes the texture greyscale
void greyscaleTexture(TEXTURE &tex);

//Show loading text above, will be cleared on the next rendered frame, doesn't really belong here
//Increases internal counter, if it's >= i it's shown, if i == -1, reset the internal counter
void drawLoadingtext(const int i);

struct TextureAtlasEntry
{
    GLFix left, right; //U
    GLFix top, bottom; //V
};

TextureAtlasEntry textureArea(const int x, const int y, const int w, const int h);

#endif // TEXTURETOOLS_H
