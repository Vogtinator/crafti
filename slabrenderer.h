#ifndef SLABRENDERER_H
#define SLABRENDERER_H

#include "blockrenderer.h"

class SlabRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return false; } // Not oriented (for extra data)
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; } // Torch-like orientation

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    // Used for particles spawned on destruction
    virtual const TerrainAtlasEntry &destructionTexture(const BLOCK_WDATA block) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    const TerrainAtlasEntry &getSlabTexture(const BLOCK_WDATA block);

    static constexpr GLFix slab_height = BLOCK_SIZE / 16 * 8;
    static constexpr GLFix slab_width = BLOCK_SIZE;

    enum SLAB_TYPE {
        SLAB_STONE=0,
        SLAB_WOOD=1,
        SLAB_PLANKS_NORMAL=2,
        SLAB_WALL=3,
        SLAB_PLANKS_DARK=4,
        SLAB_PLANKS_BRIGHT=5,
        SLAB_COBBLESTONE=6,
        SLAB_NETHERRACK=7,
    };
};

#endif // SLABRENDERER_H
