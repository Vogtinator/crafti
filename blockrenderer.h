#ifndef BLOCKRENDERER_H
#define BLOCKRENDERER_H

#include <memory>

#include "terrain.h"
#include "chunk.h"

class BlockRenderer
{
public:
    virtual ~BlockRenderer() {};

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) = 0;
    virtual int indicesNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c) = 0;
    virtual void geometryNormalBlock(const BLOCK_WDATA block, int x, int y, int z, const BLOCK_SIDE side, Chunk &c) = 0;
    virtual bool isOpaque(const BLOCK_WDATA block) = 0;
    virtual bool isObstacle(const BLOCK_WDATA block) = 0;
    virtual bool isOriented(const BLOCK_WDATA block) = 0;

    virtual bool isBlockShaped(const BLOCK_WDATA block) = 0;
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) = 0;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) = 0;

    static void renderNormalBlockSide(int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry tex, Chunk &c);
    static void renderBillboard(int local_x, int local_y, int local_z, const TextureAtlasEntry tex, Chunk &c);

    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0;
    virtual void updateBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0;
    virtual void addBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0;
    virtual void removeBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0;

    virtual const char* getName(const BLOCK_WDATA block) = 0;
};

//A proxy class which redirects calls to the appropriate BlockRenderer
class UniversalBlockRenderer : public BlockRenderer
{
public:
    UniversalBlockRenderer();

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual int indicesNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA block) override;
    virtual bool isObstacle(const BLOCK_WDATA block) override;
    virtual bool isOriented(const BLOCK_WDATA block) override;

    virtual bool isBlockShaped(const BLOCK_WDATA block) override;
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void updateBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void addBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void removeBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA block) override;

private:
    std::shared_ptr<BlockRenderer> map[256];
};

class DumbBlockRenderer : public BlockRenderer
{
    virtual void tick(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void updateBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void addBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void removeBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
};

class NormalBlockRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/, Chunk &/*c*/) override {};
    virtual int indicesNormalBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override { return 4; }
    virtual void geometryNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/) override { return {}; } //Handled by isBlockShaped

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int dest_x, int dest_y) override;

    virtual const char* getName(const BLOCK_WDATA block) override { return block_names[getBLOCK(block)]; }
};

//As placeholder, so unknown blocks won't crash the game. Also used for air
class NullBlockRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/, Chunk &/*c*/) override {};
    virtual int indicesNormalBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override { return 0; }
    virtual void geometryNormalBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {}
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/) override { return {}; }

    virtual void drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &/*dest*/, int /*dest_x*/, int /*dest_y*/) override {}

    virtual const char* getName(const BLOCK_WDATA /*block*/) override { return "NULL"; }
};

extern UniversalBlockRenderer globalBlockRenderer;

#endif // BLOCKRENDERER_H
