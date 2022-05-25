#include <cstdlib>

#include "lamprenderer.h"

void LampRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    STATE powered = static_cast<STATE>(getBLOCKDATA(block));
    TerrainAtlasEntry &tae = powered ? terrain_atlas[4][13] : terrain_atlas[3][13];

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tae.current, c);
}

void LampRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[4][13].resized, dest, x, y);
}

const TerrainAtlasEntry &LampRenderer::destructionTexture(const BLOCK_WDATA block) {
    return static_cast<STATE>(getBLOCKDATA(block)) ? terrain_atlas[4][13] : terrain_atlas[3][13];
}

void LampRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    STATE powered = c.isBlockPowered(local_x, local_y, local_z) ? ON : OFF;
    if(getBLOCKDATA(block) != powered)
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), powered));
}

const char *LampRenderer::getName(const BLOCK_WDATA)
{
    return "Redstone Lamp";
}
