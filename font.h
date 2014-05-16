#ifndef FONT_H
#define FONT_H

#include "gl.h"

void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
unsigned int fontHeight();

#endif // FONT_H
