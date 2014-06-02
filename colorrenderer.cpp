#include "colorrenderer.h"

#include "settingstask.h"
#include "terrain.h"

void ColorBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(settings_task.getValue(SettingsTask::FAST_MODE) && quad_block_textures[getBLOCK(block)][side].has_quad)
    {
        //Whether to use the dark or light block color
        bool color = (local_x + local_y + local_z) % 2;

        switch(side)
        {
        case BLOCK_BACK:
        case BLOCK_FRONT:
            if(shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y + 1, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block))
            {
                color ^= local_x & 2;
                color ^= local_y & 2;
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, color ? quad_block_textures[getBLOCK(block)][side].color : quad_block_textures[getBLOCK(block)][side].darker, c);
            }

            break;
        case BLOCK_LEFT:
        case BLOCK_RIGHT:
            if(shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block))
            {
                color ^= local_y & 2;
                color ^= local_z & 2;
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, color ? quad_block_textures[getBLOCK(block)][side].color : quad_block_textures[getBLOCK(block)][side].darker, c);
            }

            break;
        case BLOCK_BOTTOM:
        case BLOCK_TOP:
            if(shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z + 1, side, c, block)
                && shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block))
            {
                color ^= local_z & 2;
                color ^= local_x & 2;
                return BlockRenderer::renderNormalBlockSideQuadForceColor(local_x, local_y, local_z, side, color ? quad_block_textures[getBLOCK(block)][side].color : quad_block_textures[getBLOCK(block)][side].darker, c);
            }

            break;
        default:
            break;
        }

        return BlockRenderer::renderNormalBlockSideForceColor(local_x, local_y, local_z, side, color ? quad_block_textures[getBLOCK(block)][side].color : quad_block_textures[getBLOCK(block)][side].darker, c);
    }

    NormalBlockRenderer::geometryNormalBlock(block, local_x, local_y, local_z, side, c);
}
