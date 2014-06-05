#include "switchrenderer.h"

void SwitchRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    return BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[3][13].resized, false, dest, x, y);
}

bool SwitchRenderer::action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    STATE powered = static_cast<STATE>(getBLOCKDATA(block));

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, powered ? OFF : ON, !powered));

    return true;
}

const char *SwitchRenderer::getName(const BLOCK_WDATA)
{
    return "Redstone Switch";
}
