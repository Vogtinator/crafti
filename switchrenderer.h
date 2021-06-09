#ifndef SWITCHRENDERER_H
#define SWITCHRENDERER_H

#include "blockrenderer.h"
#include "torchrenderer.h"

class SwitchRenderer : public TorchRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;
    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;
    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;
    virtual const char* getName(const BLOCK_WDATA) override;
};

#endif // SWITCHRENDERER_H
