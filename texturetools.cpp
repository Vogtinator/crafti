#include <algorithm>
#include <cstdio>

#include <libndls.h>

#include "gl.h"
#include "texturetools.h"

TEXTURE* newTexture(const unsigned int w, const unsigned int h, const COLOR fill)
{
    TEXTURE *ret = new TEXTURE;

    ret->width = w;
    ret->height = h;
    ret->bitmap = new COLOR[w * h]{fill};

    return ret;
}

void deleteTexture(TEXTURE *tex)
{
    delete[] tex->bitmap;
    delete tex;
}

void copyTexture(const TEXTURE &src, TEXTURE &dest)
{
    #ifdef DEBUG
        if(src.width != dest.width || src.height != dest.height)
        {
            puts("Error: textures don't have the same resolution!");
            return;
        }
    #endif

    std::copy(src.bitmap, src.bitmap + src.width*src.height, dest.bitmap);
}

struct RGB24 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed));

bool skip_space(FILE *file)
{
    char c;
    do {
        int i = getc(file);
        if(i == EOF)
            return false;
        c = i;
    } while(c == '\n' || c == '\r' || c == '\t' || c == ' ');

    ungetc(c, file);
    return true;
}

//PPM-Loader without support for ascii
TEXTURE* loadTextureFromFile(const char* filename)
{
    FILE *texture_file = fopen(filename, "rb");
    if(!texture_file)
        return nullptr;

    char magic[3];
    magic[2] = 0;
    unsigned int width, height, pixel_max, pixels;
    RGB24 *buffer, *ptr24;
    uint16_t *ptr16;
    TEXTURE *texture = nullptr;

    if(fread(magic, 1, 2, texture_file) != 2 || strcmp(magic, "P6"))
        goto end;

    if(!skip_space(texture_file))
        goto end;

    if(fscanf(texture_file, "%d", &width) != 1)
        goto end;

    if(!skip_space(texture_file))
        goto end;

    if(fscanf(texture_file, "%d", &height) != 1)
        goto end;

    if(!skip_space(texture_file))
        goto end;

    if(fscanf(texture_file, "%d", &pixel_max) != 1 || pixel_max != 255)
        goto end;

    if(!skip_space(texture_file))
        goto end;

    texture = newTexture(width, height);
    if(!texture)
        goto end;

    pixels = width * height;
    buffer = new RGB24[pixels];
    if(!buffer)
        goto end;

    if(fread(buffer, sizeof(RGB24), pixels, texture_file) != pixels)
    {
        delete[] buffer;
        goto end;
    }

    //Convert to RGB565
    ptr24 = buffer;
    ptr16 = texture->bitmap;
    while(pixels--)
    {
        *ptr16 = (ptr24->r & 0b11111000) << 8 | (ptr24->g & 0b11111100) << 3 | (ptr24->b & 0b11111000) >> 3;
        ptr24++;
        ptr16++;
    }

    delete[] buffer;

    end:

    fclose(texture_file);

    return texture;
}

TextureAtlasEntry textureArea(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h)
{
    return TextureAtlasEntry{
        .left = x,
        .right = x+w,
        .top = y,
        .bottom = y+h,
    };
}

void drawTexture(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h)
{
    if(dest_x >= dest.width || dest_y >= dest.height)
        return;

    if(src_x >= src.width || src_y >= src.height)
        return;

    w = std::min(w, dest.width - dest_x);
    h = std::min(h, dest.height - dest_y);

    COLOR *dest_ptr = dest.bitmap + dest_x + dest_y * dest.width,
            *src_ptr = src.bitmap + src_x + src_y * src.width;

    const unsigned int nextline_dest = dest.width - w, nextline_src = src.width - w;

    for(unsigned int i = h; i--;)
    {
        for(unsigned int j = w; j--;)
            *dest_ptr++ = *src_ptr++;

        dest_ptr += nextline_dest;
        src_ptr += nextline_src;
    }
}

void drawTextureOverlay(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h)
{
    if(dest_x >= dest.width || dest_y >= dest.height)
        return;

    if(src_x >= src.width || src_y >= src.height)
        return;

    w = std::min(w, dest.width - dest_x);
    h = std::min(h, dest.height - dest_y);

    COLOR *dest_ptr = dest.bitmap + dest_x + dest_y * dest.width,
            *src_ptr = src.bitmap + src_x + src_y * src.width;

    const unsigned int nextline_dest = dest.width - w, nextline_src = src.width - w;

    for(unsigned int i = h; i--;)
    {
        for(unsigned int j = w; j--;)
        {
            COLOR src = *src_ptr++;
            COLOR *dest = dest_ptr++;

            const unsigned int r_o = (*dest >> 11) & 0b11111;
            const unsigned int g_o = (*dest >> 5) & 0b111111;
            const unsigned int b_o = (*dest >> 0) & 0b11111;

            const unsigned int r_n = (src >> 11) & 0b11111;
            const unsigned int g_n = (src >> 5) & 0b111111;
            const unsigned int b_n = (src >> 0) & 0b11111;

            //Generate 50% opacity
            unsigned int r = (r_n + r_o) >> 1;
            unsigned int g = (g_n + g_o) >> 1;
            unsigned int b = (b_n + b_o) >> 1;

            *dest = (r << 11) | (g << 5) | (b << 0);
        }

        dest_ptr += nextline_dest;
        src_ptr += nextline_src;
    }
}

void drawTransparentTexture(const TEXTURE &src, const unsigned int src_x, const unsigned int src_y, TEXTURE &dest, const unsigned int dest_x, const unsigned int dest_y, unsigned int w, unsigned int h)
{
    if(dest_x >= dest.width || dest_y >= dest.height)
        return;

    if(src_x >= src.width || src_y >= src.height)
        return;

    w = std::min(w, dest.width - dest_x);
    h = std::min(h, dest.height - dest_y);

    COLOR *dest_ptr = dest.bitmap + dest_x + dest_y * dest.width,
            *src_ptr = src.bitmap + src_x + src_y * src.width;

    const unsigned int nextline_dest = dest.width - w, nextline_src = src.width - w;

    for(unsigned int i = h; i--;)
    {
        for(unsigned int j = w; j--;)
        {
            COLOR src = *src_ptr++;
            COLOR *dest = dest_ptr++;

            if(src != 0x0000)
                *dest = src;
        }

        dest_ptr += nextline_dest;
        src_ptr += nextline_src;
    }
}

TEXTURE* resizeTexture(const TEXTURE &src, const unsigned int w, const unsigned int h)
{
    TEXTURE *ret = newTexture(w, h);

    if(w == src.width && h == src.height)
    {
        copyTexture(src, *ret);
        return ret;
    }

    GLFix srcx = 0, srcy = 0;
    const GLFix dx = GLFix(src.width) / GLFix(w), dy = GLFix(src.height) / GLFix(h);
    COLOR *ptr = ret->bitmap;

    for(unsigned int i = h; i--;)
    {
        srcx = 0;
        for(unsigned int j = w; j--;)
        {
            *ptr++ = src.bitmap[srcx.floor() + srcy.floor() * src.width];
            srcx += dx;
        }
        srcy += dy;
    }

    return ret;
}

void greyscaleTexture(TEXTURE &tex)
{
    unsigned int pixels = tex.width * tex.height;
    COLOR *ptr16 = tex.bitmap;
    while(pixels--)
    {
        const RGB rgb = rgbColor(*ptr16);
        //Somehow only the lowest 5 bits are used
        *ptr16 = (rgb.r.value + rgb.g.value + rgb.g.value + rgb.b.value) >> 5;
        ptr16++;
    }
}
