#ifndef BLOCKRENDERER_H
#define BLOCKRENDERER_H

#include <memory>

#include "terrain.h"
#include "chunk.h"

class BlockRenderer
{
public:
    virtual ~BlockRenderer() {}

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) = 0;
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int x, const int y, const int z, const BLOCK_SIDE side, Chunk &c) = 0;
    virtual bool isOpaque(const BLOCK_WDATA block) = 0; //If true, adjacent sides aren't rendered
    virtual bool isObstacle(const BLOCK_WDATA block) = 0; //Whether the player can pass through
    virtual bool isOriented(const BLOCK_WDATA block) = 0; //Whether its data is a BLOCK_SIDE
    virtual bool isFullyOriented(const BLOCK_WDATA block) = 0; //Whether its data can be BLOCK_TOP or BLOCK_BOTTOM

    virtual bool isBlockShaped(const BLOCK_WDATA block) = 0; // Whether the AABB is the full block
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) = 0;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int x, const int y) = 0;
    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry materialTexture(const BLOCK_WDATA block);

    static void renderNormalBlockSide(int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry &tex, Chunk &c, const COLOR color = 0);
    //Renders dx*dy*dz (depending on the side) block sides (max 2x2) at once
    static void renderNormalBlockSides(int local_x, int local_y, int local_z, int width, int dy, int dz, const BLOCK_SIDE side, TextureAtlasEntry tex, Chunk &c, const COLOR color = 0);
    //Renders dx*dy*dz block sides at once (depending on the side)
    static void renderNormalBlockSidesForceColor(int local_x, int local_y, int local_z, int dx, int dy, int dz, const BLOCK_SIDE side, const COLOR color, Chunk &c);
    //Doesn't render sides adjacent to other blocks of the same type, used by glass, water, leaves etc.
    static void renderNormalConnectedBlockSide(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry &tex, const COLOR col, Chunk &c);
    static void renderBillboard(int local_x, int local_y, int local_z, const TextureAtlasEntry &tex, Chunk &c);
    static void drawTextureAtlasEntry(TEXTURE &src, const TextureAtlasEntry &tex, TEXTURE &dest, const int dest_x, const int dest_y);

    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) = 0; //Invoked by e.g. a right click
    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0; //Invoked every now-and-then
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0; //Invoked if this block has been placed
    virtual void removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) = 0; //Invoked if this block has been removed

    /* Redstone */
    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) = 0; //Return the power state to the block adjacent to side.

    virtual const char* getName(const BLOCK_WDATA block) = 0;
};

//A proxy class which redirects calls to the appropriate BlockRenderer
class UniversalBlockRenderer : public BlockRenderer
{
public:
    UniversalBlockRenderer();
    virtual ~UniversalBlockRenderer();

    virtual void renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA block) override;
    virtual bool isObstacle(const BLOCK_WDATA block) override;
    virtual bool isOriented(const BLOCK_WDATA block) override;
    virtual bool isFullyOriented(const BLOCK_WDATA block) override;

    virtual bool isBlockShaped(const BLOCK_WDATA block) override;
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int x, const int y) override;
    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry materialTexture(const BLOCK_WDATA block) override;

    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;
    virtual void tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;
    virtual void removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual PowerState powersSide(const BLOCK_WDATA block, BLOCK_SIDE side) override;

    virtual const char* getName(const BLOCK_WDATA block) override;

private:
    std::shared_ptr<BlockRenderer> map[256];
};

class DumbBlockRenderer : public BlockRenderer
{
    virtual void renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/, Chunk &/*c*/) override {}
    virtual void geometryNormalBlock(const BLOCK_WDATA /*block*/, const int /*x*/, const int /*y*/, const int /*z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {}
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &/*dest*/, const int /*x*/, const int /*y*/) override { return; }
    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry materialTexture(const BLOCK_WDATA block) override;

    virtual bool action(const BLOCK_WDATA /*block*/, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, Chunk &/*c*/) override { return false; }
    virtual void tick(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void addedBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}
    virtual void removedBlock(const BLOCK_WDATA /*block*/, int /*local_x*/, int /*local_y*/, int /*local_z*/, Chunk &/*c*/) override {}

    virtual PowerState powersSide(const BLOCK_WDATA /*block*/, BLOCK_SIDE /*side*/) override { return PowerState::NotPowered; }
};

class NormalBlockRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/, Chunk &/*c*/) override {};
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int dest_x, const int dest_y) override;
    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry materialTexture(const BLOCK_WDATA block) override;

    virtual const char* getName(const BLOCK_WDATA block) override { return block_names[getBLOCK(block)]; }

    //For face-combining (see terrain.h, bottom) we need to know whether we can combine it
    bool shouldRenderFaceAndItsTheSameAs(const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c, const BLOCK_WDATA block);
};

class OrientedBlockRenderer : public NormalBlockRenderer
{
    virtual void geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }
};

//As placeholder, so unknown blocks won't crash the game. Also used for air
class NullBlockRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/, Chunk &/*c*/) override {};
    virtual void geometryNormalBlock(const BLOCK_WDATA /*block*/, const int /*local_x*/, const int /*local_y*/, const int /*local_z*/, const BLOCK_SIDE /*side*/, Chunk &/*c*/) override {}
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; }

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return true; }
    virtual AABB getAABB(const BLOCK_WDATA /*block*/, GLFix /*x*/, GLFix /*y*/, GLFix /*z*/) override { return {}; }

    virtual void drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &/*dest*/, int /*dest_x*/, int /*dest_y*/) override {}

    virtual const char* getName(const BLOCK_WDATA /*block*/) override { return "NULL"; }
};

extern UniversalBlockRenderer global_block_renderer;

#endif // BLOCKRENDERER_H
