#include <cstdlib>

#include "wheatrenderer.h"

void WheatRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    BlockRenderer::renderBillboard((x - c.absX()) / BLOCK_SIZE, (y - c.absY()) / BLOCK_SIZE, (z - c.absZ()) / BLOCK_SIZE, terrain_atlas[8 + getBLOCKDATA(block)][5].current, c);
}

AABB WheatRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    const GLFix height = BLOCK_SIZE / (max_growth + 1) * (getBLOCKDATA(block) + 1);

    return {x, y, z, x + BLOCK_SIZE, y + height, z + BLOCK_SIZE};
}

void WheatRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[12][5].resized, dest, x, y);
}

void WheatRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //If not irrigated, it's instantly withered (growth of 0)
    if(!isIrrigated(local_x, local_y, local_z, c))
        return c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), 0));

    //Grow only if lucky
    if(rand() % 30 != 0)
        return;

    const uint8_t growth = getBLOCKDATA(block);
    if(growth == max_growth)
        return;

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), growth + 1));
}

void WheatRenderer::addedBlock(const BLOCK_WDATA /*block*/, int local_x, int local_y, int local_z, Chunk &c)
{
    if(!isIrrigated(local_x, local_y, local_z, c))
        return c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
}

const char *WheatRenderer::getName(const BLOCK_WDATA)
{
    return "Wheat";
}

//To be irrigated, a block of water must be adjacent to the block underneath, which has to be dirt or grass
bool WheatRenderer::isIrrigated(const int local_x, const int local_y, const int local_z, const Chunk &c)
{
    const BLOCK underneath = c.getGlobalBlockRelative(local_x, local_y - 1, local_z);
    if(underneath != BLOCK_DIRT && underneath != BLOCK_GRASS)
        return false;

    return getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y - 1, local_z)) == BLOCK_WATER
            || getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y - 1, local_z)) == BLOCK_WATER
            || getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z - 1)) == BLOCK_WATER
            || getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z + 1)) == BLOCK_WATER;
}
