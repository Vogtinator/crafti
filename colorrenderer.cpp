#include "colorrenderer.h"

#include "settingstask.h"
#include "terrain.h"

void ColorBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(settings_task.getValue(SettingsTask::FAST_MODE) && quad_block_textures[getBLOCK(block)][side].has_quad)
        return BlockRenderer::renderNormalBlockSideForceColor(local_x, local_y, local_z, side, (local_x + local_y + local_z) % 2 ? quad_block_textures[getBLOCK(block)][side].color : quad_block_textures[getBLOCK(block)][side].darker, c);

    NormalBlockRenderer::geometryNormalBlock(block, local_x, local_y, local_z, side, c);
}
