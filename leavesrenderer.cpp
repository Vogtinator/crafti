#include "leavesrenderer.h"

void LeavesRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    const TextureAtlasEntry &tex = transparent ? terrain_atlas[4][3].current : block_textures[BLOCK_LEAVES][BLOCK_FRONT].current;

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tex, c, transparent ? TEXTURE_TRANSPARENT : 0);
}

void LeavesRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    const TextureAtlasEntry &tex = transparent ? terrain_atlas[4][3].resized : block_textures[BLOCK_LEAVES][BLOCK_FRONT].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, transparent, dest, x, y);
}
