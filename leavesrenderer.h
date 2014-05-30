#ifndef LEAVESRENDERER_H
#define LEAVESRENDERER_H

#include "blockrenderer.h"

#include "settingstask.h"

class LeavesRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override;
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;

private:
    bool isTransparent();
};

#endif // LEAVESRENDERER_H
