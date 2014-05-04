#include "glassrenderer.h"

void GlassRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    const TextureAtlasEntry &tex = block_textures[BLOCK_GLASS][BLOCK_FRONT].current;

    //Don't render sides adjacent to other glass blocks
    switch(side)
    {
    case BLOCK_TOP:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_GLASS)
            return;
        break;
    case BLOCK_BOTTOM:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == BLOCK_GLASS)
            return;
        break;
    case BLOCK_LEFT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == BLOCK_GLASS)
            return;
        break;
    case BLOCK_RIGHT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == BLOCK_GLASS)
            return;
        break;
    case BLOCK_BACK:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == BLOCK_GLASS)
            return;
        break;
    case BLOCK_FRONT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == BLOCK_GLASS)
            return;
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tex, c, TEXTURE_TRANSPARENT);
}

void GlassRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    const TextureAtlasEntry &tex = block_textures[BLOCK_GLASS][BLOCK_FRONT].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, true, dest, x, y);
}
