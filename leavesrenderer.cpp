#include "leavesrenderer.h"

void LeavesRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(isTransparent())
    {
        const TextureAtlasEntry &tex = terrain_atlas[4][3].current;
        BlockRenderer::renderNormalConnectedBlockSide(BLOCK_LEAVES, local_x, local_y, local_z, side, tex, TEXTURE_TRANSPARENT, c);
    }
    else
    {
        const TextureAtlasEntry &tex = block_textures[BLOCK_LEAVES][BLOCK_FRONT].current;
        BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tex, c, 0);
    }
}

bool LeavesRenderer::isOpaque(const BLOCK_WDATA)
{
    return !isTransparent();
}

void LeavesRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    const TextureAtlasEntry &tex = isTransparent() ? terrain_atlas[4][3].resized : block_textures[BLOCK_LEAVES][BLOCK_FRONT].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

bool LeavesRenderer::isTransparent()
{
    return settings_task.getValue(SettingsTask::LEAVES);
}
