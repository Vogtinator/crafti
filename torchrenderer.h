#ifndef TORCHRENDERER_H
#define TORCHRENDERER_H

#include "blockrenderer.h"

class TorchRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {};
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return true; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    // Render a torch attached to a block at the given side using given texture.
    // If flame is true, a flame animation is added as well.
    void renderTorch(const BLOCK_SIDE side, const GLFix x, const GLFix y, const GLFix z, TextureAtlasEntry tex, Chunk &c, bool flame = false);

    static constexpr GLFix torch_height = BLOCK_SIZE / 16 * 10;
    static constexpr GLFix torch_width = BLOCK_SIZE / 16 * 5;
};

#endif // TORCHRENDERER_H
