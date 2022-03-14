#ifndef FLUIDRENDERER_H
#define FLUIDRENDERER_H

#include "blockrenderer.h"

class FluidRenderer : public DumbBlockRenderer
{
public:
    FluidRenderer(const unsigned int tex_x, const unsigned int tex_y, const char *name);

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; } //Render blocks around fluids as you can swim in them
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, const int x, const int y) override;

    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA block) override;

protected:
    const unsigned int tex_x, tex_y;
    const char *name;
};

#endif // FLUIDRENDERER_H
