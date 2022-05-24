#ifndef WOOLRENDERER_H
#define WOOLRENDERER_H

#include "blockrenderer.h"

class WoolRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    enum COLOUR {
        WHITE=0, // white
        ORANGE=1, // orange
        MAGENTA=2, // magenta
        LIGHT_BLUE=3, // light blue
        YELLOW=4, // yellow
        LIME=5, // lime
        PINK=6, // pink
        GRAY=7, // gray
        LIGHT_GRAY=8, // light gray
        CYAN=9, // cyan
        PURPLE=10, // purple
        BLUE=11, // blue
        BROWN=12, // brown
        GREEN=13, // green
        RED=14, // red
        BLACK=15 // black
    };
};

#endif // WOOLRENDERER_H
