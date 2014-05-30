#include "glassrenderer.h"

void GlassRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    const TextureAtlasEntry &tex = block_textures[BLOCK_GLASS][BLOCK_FRONT].current;

    BlockRenderer::renderNormalConnectedBlockSide(BLOCK_GLASS, local_x, local_y, local_z, side, tex, TEXTURE_TRANSPARENT, c);
}

void GlassRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    const TextureAtlasEntry &tex = block_textures[BLOCK_GLASS][BLOCK_FRONT].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, true, dest, x, y);
}
