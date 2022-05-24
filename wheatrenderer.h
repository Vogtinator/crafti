#ifndef WHEATRENDERER_H
#define WHEATRENDERER_H

#include "blockrenderer.h"

class WheatRenderer : public BlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {};
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA block) override;
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, int x, int y) override;

    virtual bool action(const BLOCK_WDATA /*block*/, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, Chunk &/*c*/) override { return false; };
    virtual void tick(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override;
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void removedBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}

    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;

    virtual const char* getName(const BLOCK_WDATA) override;

    bool isIrrigated(const int local_x, const int local_y, const int local_z, const Chunk &c);

protected:
    constexpr static int max_growth = 7;
};

#endif // WHEATRENDERER_H
