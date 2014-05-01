#ifndef LEAVESRENDERER_H
#define LEAVESRENDERER_H

#include "blockrenderer.h"

class LeavesRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
};

#endif // LEAVESRENDERER_H
