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
    if(side == oppositeSide(attached_side))
        return PowerState::NotPowered; // Don't power the control block
    else if(attached_side == BLOCK_BOTTOM && side == BLOCK_BOTTOM)
        return PowerState::StronglyPowered; // When upside down, strongly power bottom
    else if(attached_side != BLOCK_BOTTOM && side == BLOCK_TOP)
        return PowerState::StronglyPowered; // Otherwise strongly power the top
    else
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

    bool power_from_control_block = c.gettingPowerFrom(control_x, control_y, control_z, side);

    //State unchanged
    if(getPOWERSTATE(block) != power_from_control_block)
        return;

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(getBLOCK(block), getBLOCKDATA(block), !power_from_control_block));
}
