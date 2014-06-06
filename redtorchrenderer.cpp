#include "redtorchrenderer.h"

void RedstoneTorchRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry &tex = getPOWERSTATE(block) ? terrain_atlas[3][6].current : terrain_atlas[3][7].current;

    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x + BLOCK_SIZE/2, y + BLOCK_SIZE/2, z + BLOCK_SIZE/2);

    std::vector<VERTEX> torch_vertices;
    torch_vertices.reserve(16);

    torch_vertices.push_back({0, 0, BLOCK_SIZE/2, tex.left, tex.bottom, 0xFFFF});
    torch_vertices.push_back({0, BLOCK_SIZE, BLOCK_SIZE/2, tex.left, tex.top, 0xFFFF});
    torch_vertices.push_back({BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE/2, tex.right, tex.top, 0xFFFF});
    torch_vertices.push_back({BLOCK_SIZE, 0, BLOCK_SIZE/2, tex.right, tex.bottom, 0xFFFF});

    torch_vertices.push_back({BLOCK_SIZE/2, 0, BLOCK_SIZE, tex.left, tex.bottom, 0xFFFF});
    torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, BLOCK_SIZE, tex.left, tex.top, 0xFFFF});
    torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, 0, tex.right, tex.top, 0xFFFF});
    torch_vertices.push_back({BLOCK_SIZE/2, 0, 0, tex.right, tex.bottom, 0xFFFF});

    switch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)))
    {
    default:
    case BLOCK_TOP:
        break;
    case BLOCK_BOTTOM:
        nglRotateX(180);
        break;
    case BLOCK_BACK:
        nglRotateX(45);
        break;
    case BLOCK_FRONT:
        nglRotateX(315);
        break;
    case BLOCK_LEFT:
        nglRotateZ(45);
        break;
    case BLOCK_RIGHT:
        nglRotateZ(315);
        break;
    }

    glTranslatef(-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2);

    for(auto&& v : torch_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}

void RedstoneTorchRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[3][6].resized, true, dest, x, y);
}

const char *RedstoneTorchRenderer::getName(const BLOCK_WDATA)
{
    return "Redstone Torch";
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

    bool powered = false;

    //To determine whether the control block is powered, we have to ignore the torch itself
    if(getPOWERSTATE(c.getGlobalBlockRelative(control_x, control_y + 1, control_z)) && side != BLOCK_TOP)
            powered = true;
    else if(getPOWERSTATE(c.getGlobalBlockRelative(control_x, control_y - 1, control_z)) && side != BLOCK_BOTTOM)
            powered = true;
    else if(getPOWERSTATE(c.getGlobalBlockRelative(control_x, control_y, control_z + 1)) && side != BLOCK_BACK)
            powered = true;
    else if(getPOWERSTATE(c.getGlobalBlockRelative(control_x, control_y, control_z - 1)) && side != BLOCK_FRONT)
            powered = true;
    else if(getPOWERSTATE(c.getGlobalBlockRelative(control_x - 1, control_y, control_z)) && side != BLOCK_LEFT)
            powered = true;
    else if(getPOWERSTATE(c.getGlobalBlockRelative(control_x + 1, control_y, control_z)) && side != BLOCK_RIGHT)
            powered = true;

    //State unchanged
    if(getPOWERSTATE(block) != powered)
        return;

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(getBLOCK(block), getBLOCKDATA(block), !powered));
}
