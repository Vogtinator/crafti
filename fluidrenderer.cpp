#include "fluidrenderer.h"

/* Behaviour of fluids (Water + Lava):
 * - Each block has a range assigned. Placed blocks have max range (see maxRange)
 * - If there's a floor, spread sideways into blocks with range - 1
 * - If there's no floor, spread down with same range (but not max range)
 * - If there's the same fluid above (any range), stay
 * - If not max range, blocks despawn unless there's an adjacent block with higher range
 *
 * Max range blocks are rendered as full blocks. If there's fluid above, it's also rendered
 * as a full block. Otherwise the average fluid levels is used for each corner's height.
 * Corners to full fluid blocks are always full height.
 */

FluidRenderer::FluidRenderer(const unsigned int tex_x, const unsigned int tex_y, const char *name) : tex_x(tex_x), tex_y(tex_y), name(name)
{}

constexpr uint8_t maxRange(const BLOCK_WDATA block)
{
    return getBLOCK(block) == BLOCK_WATER ? RANGE_WATER : RANGE_LAVA;
}

void FluidRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    const int local_x = x / BLOCK_SIZE, local_y = y / BLOCK_SIZE, local_z = z / BLOCK_SIZE;
    uint8_t range = getBLOCKDATA(block);
    BLOCK_WDATA block_top = c.getGlobalBlockRelative(local_x, local_y + 1, local_z);
    // Full blocks are handled by renderNormalBlock instead
    if(range == maxRange(block) || getBLOCK(block_top) == getBLOCK(block))
        return;

    const TextureAtlasEntry &tex = terrain_atlas[tex_x][tex_y].current;

    int adjRanges[3][3] = {};
    for(int dx : {-1, 0, +1})
        for(int dz : {-1, 0, +1})
        {
            BLOCK_WDATA that_block = c.getGlobalBlockRelative(local_x + dx, local_y, local_z + dz);
            if(getBLOCK(that_block) != getBLOCK(block))
                continue;

            BLOCK_WDATA that_block_above = c.getGlobalBlockRelative(local_x + dx, local_y + 1, local_z + dz);
            if(getBLOCK(that_block_above) == getBLOCK(block))
                adjRanges[dx+1][dz+1] = maxRange(block);
            else
                adjRanges[dx+1][dz+1] = getBLOCKDATA(that_block);
        }

    auto heightAtCorner = [block, &adjRanges](int x, int z) {
        int total = 0, height = 0;
        for(int dx : {0, 1})
            for(int dz : {0, 1})
            {
                int range = adjRanges[x + dx][z + dz];
                if(range == maxRange(block))
                    return GLFix(1);

                if(range)
                {
                    total++;
                    height += range;
                }
            }

        return GLFix(height) / total / maxRange(block);
    };

    BLOCK_WDATA block_left = c.getGlobalBlockRelative(local_x - 1, local_y, local_z),
            block_right = c.getGlobalBlockRelative(local_x + 1, local_y, local_z),
            block_front = c.getGlobalBlockRelative(local_x, local_y, local_z - 1),
            block_back = c.getGlobalBlockRelative(local_x, local_y, local_z + 1);

    GLFix corner_height[2][2], corner_tex_top[2][2];
    for(int dx : {0, 1})
        for(int dz : {0, 1})
        {
            GLFix ratio = heightAtCorner(dx, dz);
            corner_height[dx][dz] = ratio * BLOCK_SIZE;
            corner_tex_top[dx][dz] = GLFix(tex.bottom) - ratio * (tex.bottom - tex.top);
        }

    // Render sides if adjacent to a non-opaque block which isn't the same fluid
    if(!global_block_renderer.isOpaque(block_front) && getBLOCK(block_front) != getBLOCK(block))
    {
        c.addUnalignedVertex({x, y, z, tex.left, tex.bottom, Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.left, corner_tex_top[0][0], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.right, corner_tex_top[1][0], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.right, tex.bottom, Chunk::INDEPENDENT_TRIS});
    }

    if(!global_block_renderer.isOpaque(block_back) && getBLOCK(block_back) != getBLOCK(block))
    {
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.left, tex.bottom, Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.left, corner_tex_top[1][1], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.right, corner_tex_top[0][1], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.right, tex.bottom, Chunk::INDEPENDENT_TRIS});
    }

    if(!global_block_renderer.isOpaque(block_left) && getBLOCK(block_left) != getBLOCK(block))
    {
        c.addUnalignedVertex({x, y, z + BLOCK_SIZE, tex.left, tex.bottom, Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.left, corner_tex_top[0][1], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.right, corner_tex_top[0][0], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x, y, z, tex.right, tex.bottom, Chunk::INDEPENDENT_TRIS});
    }

    if(!global_block_renderer.isOpaque(block_right) && getBLOCK(block_right) != getBLOCK(block))
    {
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z, tex.left, tex.bottom, Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.left, corner_tex_top[1][0], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.right, corner_tex_top[1][1], Chunk::INDEPENDENT_TRIS});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.right, tex.bottom, Chunk::INDEPENDENT_TRIS});
    }

    c.addUnalignedVertex({x, y + corner_height[0][0], z, tex.left, tex.bottom, Chunk::INDEPENDENT_TRIS});
    c.addUnalignedVertex({x, y + corner_height[0][1], z + BLOCK_SIZE, tex.left, tex.top, Chunk::INDEPENDENT_TRIS});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][1], z + BLOCK_SIZE, tex.right, tex.top, Chunk::INDEPENDENT_TRIS});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + corner_height[1][0], z, tex.right, tex.bottom, Chunk::INDEPENDENT_TRIS});
}

void FluidRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    uint8_t range = getBLOCKDATA(block);
    BLOCK_WDATA block_top = c.getGlobalBlockRelative(local_x, local_y + 1, local_z);
    // A fluid block is like a normal block if it has full range
    bool render_as_full_block = (range == maxRange(block)) || (getBLOCK(block_top) == getBLOCK(block));
    if(!render_as_full_block && side != BLOCK_BOTTOM)
        return;

    //Don't render sides adjacent to other fluid blocks of the same type
    switch(side)
    {
    case BLOCK_TOP:
        if(getBLOCK(block_top) == getBLOCK(block))
            return;
        break;
    case BLOCK_BOTTOM:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == getBLOCK(block))
            return;
        break;
    case BLOCK_LEFT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == getBLOCK(block))
            return;
        break;
    case BLOCK_RIGHT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == getBLOCK(block))
            return;
        break;
    case BLOCK_BACK:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == getBLOCK(block))
            return;
        break;
    case BLOCK_FRONT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == getBLOCK(block))
            return;
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[tex_x][tex_y].current, c);
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
    if(range == maxRange(block))
        goto survive;

    //If there is any fluid above, survival doesn't depend on its range
    if(getBLOCK(block_top) == getBLOCK(block))
        goto survive;

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

    survive:

    // For Lava: Randomly spawn a particle with random size, velocity and offset
    if (getBLOCK(block) == BLOCK_LAVA && getBLOCK(block_top) == BLOCK_AIR && (rand() & 0xFF) < 20) {
        Particle p;
        p.tae = terrain_atlas[tex_x][tex_y].current;

        // Use the top left quarter of the texture
        const int tex_width = p.tae.right - p.tae.left,
                  tex_height = p.tae.bottom - p.tae.top;
        p.tae.right -= tex_width / 2;
        p.tae.bottom -= tex_height / 2;

        // Random value between 0 and max (inclusive)
        const auto randMax = [](GLFix max) { return max * (rand() & 0xFF) / 0xFF; };

        // Center of the top face (chunk relative coordinates)
        const auto aabb = this->getAABB(block, local_x * BLOCK_SIZE, local_y * BLOCK_SIZE, local_z * BLOCK_SIZE);
        auto center = VECTOR3{(aabb.low_x + aabb.high_x) / 2, aabb.low_y + (BLOCK_SIZE * getBLOCKDATA(block) / maxRange(block)), (aabb.low_z + aabb.high_z) / 2};

        p.size = randMax(15) + 10;
        p.vel = {randMax(10) - 5, randMax(8) + 4, randMax(10) - 5};
        p.pos = center;
        p.pos.x += randMax(100) - 50;
        p.pos.z += randMax(100) - 50;
        c.addParticle(p);
    }

    //Either flow downwards or spread
    if(getBLOCK(block_bottom) == BLOCK_AIR || getBLOCK(block_bottom) == getBLOCK(block))
    {
        //Flowing downwards means same range on block below, but not max range
        auto new_range = std::min(range, uint8_t(maxRange(block) - 1u));
        BLOCK_WDATA next_block = getBLOCKWDATA(getBLOCK(block), new_range);
        if(getBLOCK(block_bottom) == BLOCK_AIR || (getBLOCKDATA(block_bottom) < new_range))
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, next_block);

        return;
    }

    //Don't create any 0 blocks, they'd be invisible
    if(range > 1)
    {
        BLOCK_WDATA next_block = getBLOCKWDATA(getBLOCK(block), range - 1);

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
