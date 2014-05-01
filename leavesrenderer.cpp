#include "leavesrenderer.h"

void LeavesRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c)
{
    constexpr bool transparent = true;
    const TextureAtlasEntry &tex = transparent ? terrain_atlas[4][3].current : block_textures[BLOCK_LEAVES][BLOCK_FRONT].current;

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tex, c, transparent);
}
