#ifndef LEAVESRENDERER_H
#define LEAVESRENDERER_H

#include "blockrenderer.h"

class LeavesRenderer : public NormalBlockRenderer
{
public:
    static constexpr bool transparent = false;

    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return !transparent; }
};

#endif // LEAVESRENDERER_H
