#ifndef TEXTURETOOLS_H
#define TEXTURETOOLS_H

#include "gl.h"

//Throws if allocation failed
TEXTURE* newTexture(const unsigned int w, const unsigned int h, const COLOR fill = 0, const bool transparent = true, const COLOR transparent_color = 0);
void deleteTexture(TEXTURE *tex);

//Textures have to have the same resolution
void copyTexture(const TEXTURE &src, TEXTURE &dest);

//Returns nullptr if loading failed
TEXTURE *loadTextureFromFile(const char* filename);
bool saveTextureToFile(const TEXTURE &texture, const char* filename);

//Normal blitting
void drawTexture(const TEXTURE &src, TEXTURE &dest,
				 uint16_t src_x, uint16_t src_y, uint16_t src_w, uint16_t src_h,
				 uint16_t dest_x, uint16_t dest_y, uint16_t dest_w, uint16_t dest_h);
//50% opacity
void drawTextureOverlay(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h);
//Allocates memory for new texture, deleteTexture must be called
TEXTURE* resizeTexture(const TEXTURE &src, const unsigned int w, const unsigned int h);
//Makes the texture greyscale
void greyscaleTexture(TEXTURE &tex);

struct TextureAtlasEntry
{
    unsigned int left, right; //U
    unsigned int top, bottom; //V
};

TextureAtlasEntry textureArea(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h);

#endif // TEXTURETOOLS_H
