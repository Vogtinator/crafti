#include "gl.h"
#include "font.h"

#include "textures/font_dat.h"
#include "textures/font_bmp.h"

static inline int drawChar(char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    uint32_t width = font_dat[8];
    uint32_t height = font_dat[12];

    //font_dat[16] is the char at the top left
    const unsigned int pos = c - font_dat[16];
    const unsigned int cols = font_bmp.width / font_dat[8];
    unsigned int pos_x = pos % cols;
    unsigned int pos_y = pos / cols;
    pos_x *= width;
    pos_y *= height;

    //Each character has its specific width
    width = font_dat[c + 17];

    for(unsigned int x1 = 0; x1 < width; x1++)
        for(unsigned int y1 = 0; y1 < height; y1++)
        {
            if(font_bmp.bitmap[pos_x + x1 + (pos_y + y1) * font_bmp.width] == 0xFFFF)
                tex.bitmap[x + x1 + (y + y1) * tex.width] = color;
        }

    return width;
}

void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    unsigned int width = 0;
    const char *ptr = str;
    while(*ptr)
        width += font_dat[17 + *ptr++];

    x -= width / 2;
    drawString(str, color, tex, x, y);
}

void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    const char *ptr = str;
    const unsigned int start_x = x;
    while(*ptr)
    {
        if(*ptr == '\n')
        {
            x = start_x;
            y += fontHeight();
        }
        else if(*ptr == '\t')
        {
            x += 32;
            x -= x % 32;
        }
        else
            x += drawChar(*ptr, color, tex, x, y);

        ++ptr;
    }
}

unsigned int fontHeight()
{
    return font_dat[12];
}
