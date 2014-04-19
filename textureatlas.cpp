#include <cstdio>

#include "textureatlas.h"

struct RGB24 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed));

bool skip_space(FILE *file)
{
    char c;
    do {
        c = getc(file);
        if(c == EOF)
            return false;
    } while(c == '\n' || c == '\r' || c == '\t' || c == ' ');

    ungetc(c, file);
    return true;
}

//PPM-Loader without support for textures
bool loadTextureFromFile(const char* filename, TEXTURE *texture)
{
    FILE *texture_file = fopen(filename, "rb");
    if(!texture_file)
        return false;

    char magic[3];
    magic[2] = 0;
    int pixel_max;

    if(fread(magic, 1, 2, texture_file) != 2)
    {
        fclose(texture_file);
        return false;
    }
    if(!skip_space(texture_file))
    {
        fclose(texture_file);
        return false;
    }
    if(fscanf(texture_file, "%d", &texture->width) != 1)
    {
        fclose(texture_file);
        return false;
    }
    if(!skip_space(texture_file))
    {
        fclose(texture_file);
        return false;
    }
    if(fscanf(texture_file, "%d", &texture->height) != 1)
    {
        fclose(texture_file);
        return false;
    }
    if(!skip_space(texture_file))
    {
        fclose(texture_file);
        return false;
    }
    if(fscanf(texture_file, "%d", &pixel_max) != 1)
    {
        fclose(texture_file);
        return false;
    }
    if(!skip_space(texture_file))
    {
        fclose(texture_file);
        return false;
    }

    if(strcmp(magic, "P6") || texture->width != texture->height || pixel_max != 255)
    {
        fclose(texture_file);
        return false;
    }

    unsigned int pixels = texture->width * texture->height;
    RGB24 *buffer = new RGB24[pixels];
    if(fread(buffer, sizeof(RGB24), pixels, texture_file) != pixels)
    {
        delete[] buffer;
        fclose(texture_file);
        return false;
    }

    //Convert to RGB565
    texture->bitmap = new uint16_t[pixels];
    RGB24 *ptr24 = buffer;
    uint16_t *ptr16 = texture->bitmap;
    while(pixels--)
    {
        *ptr16 = (ptr24->r >> 3 & 0b11111) << 11 | (ptr24->g >> 2 & 0b111111) << 5 | (ptr24->b >> 3 & 0x11111);
        ptr24++;
        ptr16++;
    }

    delete[] buffer;

    fclose(texture_file);

    return true;
}

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
