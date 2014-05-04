#include "fluidrenderer.h"

FluidRenderer::FluidRenderer(const unsigned int tex_x, const unsigned int tex_y, const char *name) : tex_x(tex_x), tex_y(tex_y), name(name)
{}

void FluidRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    if(range == (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA))
        return;

    const TextureAtlasEntry &tex = terrain_atlas[tex_x][tex_y].current;

    //Height is proportional to its range
    const GLFix ratio = GLFix(getBLOCKDATA(block)) / (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA);
    GLFix height = GLFix(BLOCK_SIZE) * ratio, tex_top = tex.bottom - (tex.bottom - tex.top) * ratio;

    c.addUnalignedVertex({x, y, z, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x, y + height, z, tex.left, tex_top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z, tex.right, tex_top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.right, tex.bottom, 0});

    c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z + BLOCK_SIZE, tex.left, tex_top, 0});
    c.addUnalignedVertex({x, y + height, z + BLOCK_SIZE, tex.right, tex_top, 0});
    c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.right, tex.bottom, 0});

    c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x, y + height, z + BLOCK_SIZE, tex.left, tex_top, 0});
    c.addUnalignedVertex({x, y + height, z, tex.right, tex_top, 0});
    c.addUnalignedVertex({x, y, z, tex.right, tex.bottom, 0});

    c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z, tex.left, tex_top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z + BLOCK_SIZE, tex.right, tex_top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.right, tex.bottom, 0});

    c.addUnalignedVertex({x, y + height, z, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x, y + height, z + BLOCK_SIZE, tex.left, tex.top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z + BLOCK_SIZE, tex.right, tex.top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + height, z, tex.right, tex.bottom, 0});
}

void FluidRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    if(range == (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA))
        return BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[tex_x][tex_y].current, c);

    if(side != BLOCK_BOTTOM)
        return;

    if(global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)))
        return;

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, BLOCK_BOTTOM, terrain_atlas[tex_x][tex_y].current, c);
}

bool FluidRenderer::isOpaque(const BLOCK_WDATA block)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    return range == (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA);
}

bool FluidRenderer::isBlockShaped(const BLOCK_WDATA block)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    return range == (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA);
}

AABB FluidRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    //Height is proportional to its range
    const GLFix ratio = GLFix(getBLOCKDATA(block)) / (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA);
    GLFix height = GLFix(BLOCK_SIZE) * ratio;

    return {x, y, z, x + BLOCK_SIZE, y + height, z + BLOCK_SIZE};
}

void FluidRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, const int x, const int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[tex_x][tex_y].resized, false, dest, x, y);
}

void FluidRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    if(range == 0)
        return;

    BLOCK_WDATA block_left = c.getGlobalBlockRelative(local_x - 1, local_y, local_z),
            block_right = c.getGlobalBlockRelative(local_x + 1, local_y, local_z),
            block_front = c.getGlobalBlockRelative(local_x, local_y, local_z - 1),
            block_back = c.getGlobalBlockRelative(local_x, local_y, local_z + 1);

    //If a block doesn't have the full range, it despawns without a adjacent block > range
    if(range != (getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA))
    {
        if(getBLOCK(block_left) == getBLOCK(block) && getBLOCKDATA(block_left) > range)
            goto survive;
        if(getBLOCK(block_right) == getBLOCK(block) && getBLOCKDATA(block_right) > range)
            goto survive;
        if(getBLOCK(block_front) == getBLOCK(block) && getBLOCKDATA(block_front) > range)
            goto survive;
        if(getBLOCK(block_back) == getBLOCK(block) && getBLOCKDATA(block_back) > range)
            goto survive;

        //Remove myself, I'm dead :-(
        c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        return;
    }

    survive:

    if(range == 1) //Don't create any 0 blocks, they'd be invisible
        return;

    BLOCK_WDATA next_block = getBLOCKWDATA(getBLOCK(block), range - 1);

    if(getBLOCK(block_left) == BLOCK_AIR)
        c.setGlobalBlockRelative(local_x - 1, local_y, local_z, next_block);
    if(getBLOCK(block_right) == BLOCK_AIR)
        c.setGlobalBlockRelative(local_x + 1, local_y, local_z, next_block);
    if(getBLOCK(block_front) == BLOCK_AIR)
        c.setGlobalBlockRelative(local_x, local_y, local_z - 1, next_block);
    if(getBLOCK(block_back) == BLOCK_AIR)
        c.setGlobalBlockRelative(local_x, local_y, local_z + 1, next_block);

    //Flowing downwards means full range on block below
    if(c.getGlobalBlockRelative(local_x, local_y - 1, local_z) == BLOCK_AIR)
    {
        next_block = getBLOCKWDATA(getBLOCK(block), getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA);
        c.setGlobalBlockRelative(local_x, local_y - 1, local_z, next_block);
    }
}

const char *FluidRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return name;
}
