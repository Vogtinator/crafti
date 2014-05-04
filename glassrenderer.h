#ifndef GLASSRENDERER_H
#define GLASSRENDERER_H

#include "blockrenderer.h"

class GlassRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
};

#endif // GLASSRENDERER_H
