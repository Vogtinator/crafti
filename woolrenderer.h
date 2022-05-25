#ifndef WOOLRENDERER_H
#define WOOLRENDERER_H

#include "blockrenderer.h"

class WoolRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry &materialTexture(const BLOCK_WDATA block) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    enum COLOUR {
        WHITE_WOOL=0, // white
        ORANGE_WOOL=1, // orange
        MAGENTA_WOOL=2, // magenta
        LIGHT_BLUE_WOOL=3, // light blue
        YELLOW_WOOL=4, // yellow
        LIME_WOOL=5, // lime
        PINK_WOOL=6, // pink
        GRAY_WOOL=7, // gray
        LIGHT_GRAY_WOOL=8, // light gray
        CYAN_WOOL=9, // cyan
        PURPLE_WOOL=10, // purple
        BLUE_WOOL=11, // blue
        BROWN_WOOL=12, // brown
        GREEN_WOOL=13, // green
        RED_WOOL=14, // red
        BLACK_WOOL=15 // black
    };
};

#endif // WOOLRENDERER_H
