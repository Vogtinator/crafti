#ifndef SWITCHRENDERER_H
#define SWITCHRENDERER_H

#include "blockrenderer.h"
#include "lamprenderer.h"

class SwitchRenderer : public LampRenderer
{
public:
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;
    virtual void tick(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {};
    virtual const char* getName(const BLOCK_WDATA) override;
};

#endif // SWITCHRENDERER_H
