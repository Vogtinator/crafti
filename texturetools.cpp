#include <cstdio>
#include <libndls.h>

#include "texturetools.h"

//Texture with "Loading" written on it
#include "loadingtext.h"

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
        *ptr16 = (ptr24->r & 0b11111000) << 8 | (ptr24->g & 0b11111100) << 3 | (ptr24->b & 0b11111000) >> 3;
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

void drawTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h)
{
    for(int y = 0; y < h; y++)
        for(int x = 0; x < w; x++)
            dest.bitmap[dest_x + x + (dest_y + y)*dest.width] = src.bitmap[src_x + x + (src_y + y)*src.width];
}

void drawTextureOverlay(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h)
{
    for(int y = 0; y < h; y++)
        for(int x = 0; x < w; x++)
        {
            COLOR *old_c = dest.bitmap + (dest_x + x + (dest_y + y)*dest.width);
            COLOR new_c = src.bitmap[src_x + x + (src_y + y)*src.width];

            const unsigned int r_o = (*old_c >> 11) & 0b11111;
            const unsigned int g_o = (*old_c >> 5) & 0b111111;
            const unsigned int b_o = (*old_c >> 0) & 0b11111;

            const unsigned int r_n = (new_c >> 11) & 0b11111;
            const unsigned int g_n = (new_c >> 5) & 0b111111;
            const unsigned int b_n = (new_c >> 0) & 0b11111;

            unsigned int r = (r_n + r_o) >> 1;
            unsigned int g = (g_n + g_o) >> 1;
            unsigned int b = (b_n + b_o) >> 1;

            *old_c = (r << 11) | (g << 5) | (b << 0);
        }
}

void drawTransparentTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h)
{
    for(int y = 0; y < h; y++)
        for(int x = 0; x < w; x++)
        {
            COLOR c = src.bitmap[src_x + x + (src_y + y)*src.width];
            if(c != 0x0000)
                dest.bitmap[dest_x + x + (dest_y + y)*dest.width] = c;
        }
}

void drawLoadingtext(int i)
{
    static int count = 0;
    static bool shown = false;

    if(i == -1)
    {
        count = 0;
        shown = false;
        return;
    }

    if(shown == true)
        return;

    count += 1;
    if(count < i)
        return;

    shown = true;

    TEXTURE screen;
    screen.width = SCREEN_WIDTH;
    screen.height = SCREEN_HEIGHT;
    screen.bitmap = reinterpret_cast<COLOR*>(SCREEN_BASE_ADDRESS);
    drawTextureOverlay(loadingtext, 0, 0, screen, (SCREEN_WIDTH - loadingtext.width) / 2, 0, loadingtext.width, loadingtext.height);
}
