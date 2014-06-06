#ifndef TNTRENDERER_H
#define TNTRENDERER_H

#include "blockrenderer.h"

class TNTRenderer : public NormalBlockRenderer
{
public:
    virtual void tick(const BLOCK_WDATA, int local_x, int local_y, int local_z, Chunk &c) override;

protected:
    void explode(const int local_x, const int local_y, const int local_z, Chunk &c);
};

#endif // TNTRENDERER_H
