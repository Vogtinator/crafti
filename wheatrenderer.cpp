#include <cstdlib>

#include "wheatrenderer.h"

void WheatRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    BlockRenderer::renderBillboard(x / BLOCK_SIZE, y / BLOCK_SIZE, z / BLOCK_SIZE, terrain_atlas[8 + getBLOCKDATA(block)][5].current, c);
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
    //If not irrigated, it's instantly broken
    if(!isIrrigated(local_x, local_y, local_z, c))
        return c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);

    const uint8_t growth = getBLOCKDATA(block);
    if(growth == max_growth)
        return;

    //Grow only if lucky
    if(rand() % 30 != 0)
        return;

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), growth + 1));
}

void WheatRenderer::addedBlock(const BLOCK_WDATA /*block*/, int local_x, int local_y, int local_z, Chunk &c)
{
    if(!isIrrigated(local_x, local_y, local_z, c))
        return c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
}

bool WheatRenderer::isBlockShaped(const BLOCK_WDATA block) {
    const uint8_t growth = getBLOCKDATA(block);

    return growth == max_growth;
}



PowerState WheatRenderer::powersSide(const BLOCK_WDATA /*block*/, BLOCK_SIDE /*side*/)
{
    return PowerState::NotPowered;
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
