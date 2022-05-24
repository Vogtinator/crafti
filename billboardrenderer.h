#ifndef BILLBOARDRENDERER_H
#define BILLBOARDRENDERER_H

#include <map>
#include <tuple>

#include "blockrenderer.h"

class BillboardRenderer : public DumbBlockRenderer
{
public:
    BillboardRenderer() : map() {}

    void setEntry(uint8_t data, unsigned int tex_x, unsigned int tex_y, const char *name, const GLFix w, const GLFix h, const GLFix l);

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {}
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, const int y) override;

    virtual const char* getName(const BLOCK_WDATA block) override;

private:
    std::map<uint8_t, std::tuple<unsigned int, unsigned int, const char*, GLFix, GLFix, GLFix>> map; //Maps block_data to texture x, y, name, and size (w/h/l)
};

#endif // BILLBOARDRENDERER_H
