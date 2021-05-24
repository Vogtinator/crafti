#include "fluidrenderer.h"

FluidRenderer::FluidRenderer(const unsigned int tex_x, const unsigned int tex_y, const char *name) : tex_x(tex_x), tex_y(tex_y), name(name)
{}

constexpr uint8_t maxRange(const BLOCK_WDATA block)
{
    return getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA;
}

void FluidRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    if(range == maxRange(block))
        return;

    const TextureAtlasEntry &tex = terrain_atlas[tex_x][tex_y].current;

    const int local_x = x / BLOCK_SIZE, local_y = y / BLOCK_SIZE, local_z = z / BLOCK_SIZE;

    int adjRanges[3][3];
    for(int dx : {-1, 0, +1})
        for(int dz : {-1, 0, +1})
        {
            BLOCK_WDATA block = c.getGlobalBlockRelative(local_x + dx, local_y, local_z + dz);
            if(getBLOCK(block) != block)
                adjRanges[dx+1][dz+1] = 0;
            else
                adjRanges[dx+1][dz+1] = getBLOCKDATA(block);
        }

    auto heightAtCorner = [block, &adjRanges](int x, int z) {
        int total = 0, height = 0;
        for(int dx : {0, 1})
            for(int dz : {0, 1})
            {
                int range = adjRanges[x + dx][z + dz];
                if(range)
                {
                    total++;
                    height += range;
                }
            }

        return GLFix(height) / total / maxRange(block);
    };

    GLFix corner_height[2][2], corner_tex_top[2][2];
    for(int dx : {0, 1})
        for(int dz : {0, 1})
        {
            GLFix ratio = heightAtCorner(dx, dz);
            corner_height[dx][dz] = ratio * BLOCK_SIZE;
            corner_tex_top[dx][dz] = GLFix(tex.bottom) - ratio * (tex.bottom - tex.top);
        }

    BLOCK_WDATA block_left = c.getGlobalBlockRelative(local_x - 1, local_y, local_z),
            block_right = c.getGlobalBlockRelative(local_x + 1, local_y, local_z),
            block_front = c.getGlobalBlockRelative(local_x, local_y, local_z - 1),
            block_back = c.getGlobalBlockRelative(local_x, local_y, local_z + 1);

    //A liquid block with >= range as this block is like an opaque block to this block
    if(!global_block_renderer.isOpaque(block_front) && getBLOCK(block_front) != getBLOCK(block))
    {
        c.addUnalignedVertex({x, y, z, tex.left, tex.bottom, 0});
        c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.left, corner_tex_top[0][0], 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.right, corner_tex_top[1][0], 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.right, tex.bottom, 0});
    }

    if(!global_block_renderer.isOpaque(block_back) && getBLOCK(block_back) != getBLOCK(block))
    {
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.left, tex.bottom, 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.left, corner_tex_top[1][1], 0});
        c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.right, corner_tex_top[0][1], 0});
        c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.right, tex.bottom, 0});
    }

    if(!global_block_renderer.isOpaque(block_left) && getBLOCK(block_left) != getBLOCK(block))
    {
        c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.left, tex.bottom, 0});
        c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.left, corner_tex_top[0][1], 0});
        c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.right, corner_tex_top[0][0], 0});
        c.addUnalignedVertex({x, y, z, tex.right, tex.bottom, 0});
    }

    if(!global_block_renderer.isOpaque(block_right) && getBLOCK(block_right) != getBLOCK(block))
    {
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.left, tex.bottom, 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.left, corner_tex_top[1][0], 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.right, corner_tex_top[1][1], 0});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.right, tex.bottom, 0});
    }

    c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.left, tex.bottom, 0});
    c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.left, tex.top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.right, tex.top, 0});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.right, tex.bottom, 0});
}

void FluidRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    if(range != maxRange(block) && side != BLOCK_BOTTOM)
        return;

    //Don't render sides adjacent to other water blocks with full range
    switch(side)
    {
    case BLOCK_TOP:
        if(c.getGlobalBlockRelative(local_x, local_y + 1, local_z) == block)
            return;
        break;
    case BLOCK_BOTTOM:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == getBLOCK(block))
            return;
        break;
    case BLOCK_LEFT:
        if(c.getGlobalBlockRelative(local_x - 1, local_y, local_z) == block)
            return;
        break;
    case BLOCK_RIGHT:
        if(c.getGlobalBlockRelative(local_x + 1, local_y, local_z) == block)
            return;
        break;
    case BLOCK_BACK:
        if(c.getGlobalBlockRelative(local_x, local_y, local_z + 1) == block)
            return;
        break;
    case BLOCK_FRONT:
        if(c.getGlobalBlockRelative(local_x, local_y, local_z - 1) == block)
            return;
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[tex_x][tex_y].current, c);
}

bool FluidRenderer::isBlockShaped(const BLOCK_WDATA block)
{
    uint8_t range = getBLOCKDATA(block);
    //A fluid block is like a normal block if it has full range
    return range == maxRange(block);
}

AABB FluidRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    //Height is proportional to its range
    const GLFix ratio = GLFix(getBLOCKDATA(block)) / maxRange(block);
    GLFix height = GLFix(BLOCK_SIZE) * ratio;

    return {x, y, z, x + BLOCK_SIZE, y + height, z + BLOCK_SIZE};
}

void FluidRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, const int x, const int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[tex_x][tex_y].resized, dest, x, y);
}

void FluidRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    if(range == 0)
        return;

    BLOCK_WDATA block_left = c.getGlobalBlockRelative(local_x - 1, local_y, local_z),
            block_right = c.getGlobalBlockRelative(local_x + 1, local_y, local_z),
            block_top = c.getGlobalBlockRelative(local_x, local_y + 1, local_z),
            block_bottom = c.getGlobalBlockRelative(local_x, local_y - 1, local_z),
            block_front = c.getGlobalBlockRelative(local_x, local_y, local_z - 1),
            block_back = c.getGlobalBlockRelative(local_x, local_y, local_z + 1);

    //If a block doesn't have the full range, it despawns without an adjacent block > range
    if(range != maxRange(block))
    {
        if(getBLOCK(block_left) == getBLOCK(block) && getBLOCKDATA(block_left) > range)
            goto survive;
        if(getBLOCK(block_right) == getBLOCK(block) && getBLOCKDATA(block_right) > range)
            goto survive;
        if(getBLOCK(block_front) == getBLOCK(block) && getBLOCKDATA(block_front) > range)
            goto survive;
        if(getBLOCK(block_back) == getBLOCK(block) && getBLOCKDATA(block_back) > range)
            goto survive;

        //If there is any water above, survival doesn't depend on its range
        if(getBLOCK(block_top) == getBLOCK(block))
            goto survive;

        //Remove myself, I'm dead :-(
        c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        return;
    }

    survive:

    if(range == 1) //Don't create any 0 blocks, they'd be invisible
        return;

    BLOCK_WDATA next_block = getBLOCKWDATA(getBLOCK(block), range - 1);

    //Either flow downwards or spread
    if(getBLOCK(block_bottom) == BLOCK_AIR || getBLOCK(block_bottom) == getBLOCK(block))
    {
        //Flowing downwards means full range on block below
        if(getBLOCK(block_bottom) == BLOCK_AIR || (getBLOCKDATA(block_bottom) != maxRange(block)))
        {
            next_block = getBLOCKWDATA(getBLOCK(block), maxRange(block));
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, next_block);
        }
    }
    else
    {
        if(getBLOCK(block_left) == BLOCK_AIR || (getBLOCK(block_left) == getBLOCK(block) && getBLOCKDATA(block_left) < range - 1))
            c.setGlobalBlockRelative(local_x - 1, local_y, local_z, next_block);
        if(getBLOCK(block_right) == BLOCK_AIR || (getBLOCK(block_right) == getBLOCK(block) && getBLOCKDATA(block_right) < range - 1))
            c.setGlobalBlockRelative(local_x + 1, local_y, local_z, next_block);
        if(getBLOCK(block_front) == BLOCK_AIR || (getBLOCK(block_front) == getBLOCK(block) && getBLOCKDATA(block_front) < range - 1))
            c.setGlobalBlockRelative(local_x, local_y, local_z - 1, next_block);
        if(getBLOCK(block_back) == BLOCK_AIR || (getBLOCK(block_back) == getBLOCK(block) && getBLOCKDATA(block_back) < range - 1))
            c.setGlobalBlockRelative(local_x, local_y, local_z + 1, next_block);
    }
}

const char *FluidRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return name;
}
