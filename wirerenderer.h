#ifndef WIRERENDERER_H
#define WIRERENDERER_H

#include "blockrenderer.h"

class WireRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {}
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/) override;

    virtual void drawPreview(const BLOCK_WDATA, TEXTURE &dest, const int x, const int y) override;

    virtual void removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;
    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    void setCircuitState(const bool state, const int local_x, const int local_y, const int local_z, Chunk &c);
    bool isActiveLeft(const int local_x, const int local_y, const int local_z, Chunk &c);

    static constexpr uint8_t ACTIVE_BIT = 1 << 6, VISITED_BIT = 1 << 5;
    constexpr static GLFix height = BLOCK_SIZE / 16;
};

#endif // WIRERENDERER_H
