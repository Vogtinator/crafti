#ifndef PRESSURERENDERER_H
#define PRESSURERENDERER_H

#include "blockrenderer.h"

class PressurePlateRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z, Chunk &c) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void tick(const BLOCK_WDATA, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    constexpr static GLFix height = BLOCK_SIZE / 16;

};

#endif // PRESSURERENDERER_H
