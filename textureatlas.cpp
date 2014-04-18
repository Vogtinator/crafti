#include "textureatlas.h"

TextureAtlasEntry textureArea(const int x, const int y, const int w, const int h)
{
    //TODO: Remove this (very ugly) hack
    return TextureAtlasEntry{
        .left = x+1,
        .right = x+w-1,
        .top = y+1,
        .bottom = y+h-1,
    };
}
