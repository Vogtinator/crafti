#ifndef DOORRENDERER_H
#define DOORRENDERER_H

#include "blockrenderer.h"

class DoorRenderer : public BlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA, GLFix, GLFix, GLFix, Chunk &) override {};
    virtual int indicesNormalBlock(const BLOCK_WDATA, const int, const int, const int, const BLOCK_SIDE, Chunk &) override { return 0; }
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;
    virtual void tick(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void updateBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    static constexpr GLFix door_depth = BLOCK_SIZE/100; //As small as possible, a opened door shouldn't be much of an obstacle
};

#endif // DOORRENDERER_H
