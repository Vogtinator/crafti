#ifndef LAMPRENDERER_H
#define LAMPRENDERER_H

#include "blockrenderer.h"

class LampRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
     // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry &materialTexture(const BLOCK_WDATA block) override;
    
    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    enum STATE {
        OFF=0,
        ON
    };
};

#endif // LAMPRENDERER_H
