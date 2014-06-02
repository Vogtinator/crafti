#ifndef COLORRENDERER_H
#define COLORRENDERER_H

#include "blockrenderer.h"

class ColorBlockRenderer : public NormalBlockRenderer
{
public:
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
};

#endif // COLORRENDERER_H
