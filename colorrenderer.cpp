#include "colorrenderer.h"

#include "settingstask.h"
#include "terrain.h"

void ColorBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(settings_task.getValue(SettingsTask::FAST_MODE) && quad_block_textures[getBLOCK(block)][side].has_quad)
    {
        //Which shade of the color to use
        int colorIdx = (local_x ^ local_y ^ local_z ^ c.x ^ c.y ^ c.z) % (sizeof(TerrainQuadEntry::colors)/sizeof(TerrainQuadEntry::colors[0]));
        COLOR color = quad_block_textures[getBLOCK(block)][side].colors[colorIdx];

        switch(side)
        {
        case BLOCK_BACK:
        case BLOCK_FRONT:
        {
            bool can_width = shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block),
                 can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block);
            if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y + 1, local_z, side, c, block))
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 2, 2, 1, side, color, c);
            else if(can_width)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 2, 1, 1, side, color, c);
            else if(can_height)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 2, 1, side, color, c);

            break;
        }
        case BLOCK_LEFT:
        case BLOCK_RIGHT:
        {
            bool can_width = shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block),
                 can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block);
            if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z + 1, side, c, block))
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 2, 2, side, color, c);
            else if(can_width)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 1, 2, side, color, c);
            else if(can_height)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 2, 1, side, color, c);

            break;
        }
        case BLOCK_BOTTOM:
        case BLOCK_TOP:
        {
            bool can_width = shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block),
                 can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block);
            if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z + 1, side, c, block))
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 2, 1, 2, side, color, c);
            else if(can_width)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 2, 1, 1, side, color, c);
            else if(can_height)
                return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 1, 2, side, color, c);

            break;
        }
        default:
            break;
        }

        return BlockRenderer::renderNormalBlockSidesForceColor(local_x, local_y, local_z, 1, 1, 1, side, color, c);
    }

    NormalBlockRenderer::geometryNormalBlock(block, local_x, local_y, local_z, side, c);
}
