#include "redtorchrenderer.h"

void RedstoneTorchRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry &tex = getPOWERSTATE(block) ? terrain_atlas[3][6].current : terrain_atlas[3][7].current;

    TorchRenderer::renderTorch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)), x, y, z, tex, c);
}

void RedstoneTorchRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[3][6].resized, dest, x, y);
}

const char *RedstoneTorchRenderer::getName(const BLOCK_WDATA)
{
    return "Redstone Torch";
}

PowerState RedstoneTorchRenderer::powersSide(const BLOCK_WDATA block, BLOCK_SIDE side)
{
    if(!getPOWERSTATE(block))
        return PowerState::NotPowered;

    BLOCK_SIDE attached_side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block));
    if(side == attached_side)
        return PowerState::StronglyPowered;
    else if(side == oppositeSide(attached_side))
        return PowerState::NotPowered;

    return PowerState::Powered;
}

void RedstoneTorchRenderer::tick(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    int control_x = local_x, control_y = local_y, control_z = local_z;

    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block));
    switch(side)
    {
    default:
    case BLOCK_TOP:
        --control_y;
        break;
    case BLOCK_BOTTOM:
        ++control_y;
        break;
    case BLOCK_BACK:
        --control_z;
        break;
    case BLOCK_FRONT:
        ++control_z;
        break;
    case BLOCK_LEFT:
        ++control_x;
        break;
    case BLOCK_RIGHT:
        --control_x;
        break;
    }

    bool control_powered = c.isBlockPowered(control_x, control_y, control_z);

    //State unchanged
    if(getPOWERSTATE(block) != control_powered)
        return;

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(getBLOCK(block), getBLOCKDATA(block), !control_powered));
}
