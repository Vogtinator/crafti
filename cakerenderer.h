#ifndef CAKERENDERER_H
#define CAKERENDERER_H

#include "blockrenderer.h"

class CakeRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    static constexpr GLFix cake_height = BLOCK_SIZE / 16 * 9;
    static constexpr GLFix cake_width = BLOCK_SIZE / 16 * 15;
};

#endif // CAKERENDERER_H
