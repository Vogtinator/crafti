#include "colorrenderer.h"

#include "settingstask.h"
#include "terrain.h"

void ColorBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(settings_task.getValue(SettingsTask::FAST_MODE) && quad_block_textures[getBLOCK(block)][side].has_quad)
    {
        //Which shade of the color to use
        int color = (local_x ^ local_y ^ local_z ^ c.x ^ c.y ^ c.z) % (sizeof(TerrainQuadEntry::colors)/sizeof(TerrainQuadEntry::colors[0]));

        switch(side)
        {
        case BLOCK_BACK:
        case BLOCK_FRONT:
            if(shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y + 1, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block))
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, quad_block_textures[getBLOCK(block)][side].colors[color], c);

            break;
        case BLOCK_LEFT:
        case BLOCK_RIGHT:
            if(shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block))
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, quad_block_textures[getBLOCK(block)][side].colors[color], c);

            break;
        case BLOCK_BOTTOM:
        case BLOCK_TOP:
            if(shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block))
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, quad_block_textures[getBLOCK(block)][side].colors[color], c);

            break;
        default:
            break;
        }

        return BlockRenderer::renderNormalBlockSideForceColor(local_x, local_y, local_z, side, quad_block_textures[getBLOCK(block)][side].colors[color], c);
    }

    NormalBlockRenderer::geometryNormalBlock(block, local_x, local_y, local_z, side, c);
}
