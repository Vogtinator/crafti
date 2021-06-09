#ifndef DOORRENDERER_H
#define DOORRENDERER_H

#include "blockrenderer.h"

class DoorRenderer : public BlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {};
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;
    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    void toggleState(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c, const uint8_t open_state);

    static constexpr GLFix door_depth = 3; //As small as possible, a opened door shouldn't be much of an obstacle
    static constexpr uint8_t DOOR_TOP = 1 << 3, DOOR_OPEN = 1 << 4, DOOR_FORCE_OPEN = 1 << 5; //FORCE_OPEN: Opened by hand, not redstone
};

#endif // DOORRENDERER_H
