#ifndef TEXTURETOOLS_H
#define TEXTURETOOLS_H

#include "gl.h"

//Throws if allocation failed
TEXTURE* newTexture(const unsigned int w, const unsigned int h, const COLOR fill = 0);
void deleteTexture(TEXTURE *tex);

//Textures have to have the same resolution
void copyTexture(const TEXTURE &src, TEXTURE &dest);

//Returns nullptr if loading failed
TEXTURE *loadTextureFromFile(const char* filename);
bool saveTextureToFile(const TEXTURE &texture, const char* filename);

//Normal blitting
void drawTexture(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h);
//50% opacity
void drawTextureOverlay(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h);
//Black parts not drawn
void drawTransparentTexture(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h);
//Allocates memory for new texture, deleteTexture must be called
TEXTURE* resizeTexture(const TEXTURE &src, const unsigned int w, const unsigned int h);
//Makes the texture greyscale
void greyscaleTexture(TEXTURE &tex);

//Show loading text above, will be cleared on the next rendered frame, doesn't really belong here
//Increases internal counter, if it's >= i it's shown, if i == -1, reset the internal counter
void drawLoadingtext(const int i);

struct TextureAtlasEntry
{
    unsigned int left, right; //U
    unsigned int top, bottom; //V
};

TextureAtlasEntry textureArea(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h);

#endif // TEXTURETOOLS_H
