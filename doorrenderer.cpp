#include "doorrenderer.h"

void DoorRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    const BLOCK_SIDE side = static_cast<BLOCK_SIDE>((getBLOCKDATA(block) & ~(1 << 7)) ^ 1);
    const TextureAtlasEntry &tex = (getBLOCKDATA(block) & (1 << 7)) ? terrain_atlas[1][5].current : terrain_atlas[1][6].current;
    const GLFix door_offset = door_depth;

    switch(side)
    {
    case BLOCK_FRONT:
        c.addUnalignedVertex({x, y, z + door_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x, y + BLOCK_SIZE, z + door_offset, tex.left, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + BLOCK_SIZE, z + door_offset, tex.right, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + door_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        break;
    case BLOCK_BACK:
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z - door_offset + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + BLOCK_SIZE, z - door_offset + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x, y + BLOCK_SIZE, z - door_offset + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x, y, z - door_offset + BLOCK_SIZE, tex.right, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        break;
    case BLOCK_LEFT:
        c.addUnalignedVertex({x + door_offset, y, z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + door_offset, y + BLOCK_SIZE, z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + door_offset, y + BLOCK_SIZE, z, tex.right, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x + door_offset, y, z, tex.right, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        break;
    case BLOCK_RIGHT:
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y, z, tex.left, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y + BLOCK_SIZE, z, tex.left, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT | 0xFFF});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.right, tex.bottom, TEXTURE_TRANSPARENT | 0xFFF});
    default:
        break;
    }
}

AABB DoorRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>((getBLOCKDATA(block) & ~(1 << 7)) ^ 1);
    switch(side)
    {
    case BLOCK_FRONT:
        return {x, y, z, x + BLOCK_SIZE, y + BLOCK_SIZE, z + door_depth};
        break;
    case BLOCK_RIGHT:
        return {x + BLOCK_SIZE - door_depth, y, z, x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
        break;
    case BLOCK_LEFT:
        return {x, y, z, x + door_depth, y + BLOCK_SIZE, z + BLOCK_SIZE};
        break;
    case BLOCK_BACK:
        return {x, y, z + BLOCK_SIZE - door_depth, x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
        break;
    default:
        return {};
        break;
    }
}

void DoorRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    TextureAtlasEntry tex = terrain_atlas[1][5].resized;
    tex.bottom += tex.bottom - tex.top; //Double height
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, true, dest, x, y);
}

bool DoorRenderer::action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & ~(1 << 7));
    switch(side)
    {
    case BLOCK_FRONT:
        side = BLOCK_RIGHT;
        break;
    case BLOCK_RIGHT:
        side = BLOCK_FRONT;
        break;
    case BLOCK_LEFT:
        side = BLOCK_BACK;
        break;
    case BLOCK_BACK:
        side = BLOCK_LEFT;
        break;
    default:
        break;
    }

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), side | (getBLOCKDATA(block) & 1<<7)));

    //Change the other door part as well
    if(getBLOCKDATA(block) & 1<<7) //If top
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, getBLOCKWDATA(getBLOCK(block), side | ((getBLOCKDATA(block) & 1<<7) ^ 1<<7)));
    else
            c.setGlobalBlockRelative(local_x, local_y + 1, local_z, getBLOCKWDATA(getBLOCK(block), side | ((getBLOCKDATA(block) & 1<<7) ^ 1<<7)));

    return true;
}

void DoorRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getBLOCKDATA(block) & 1<<7) //If top
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) != BLOCK_AIR)
            c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        else
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, getBLOCKWDATA(getBLOCK(block), getBLOCKDATA(block) ^ 1<<7));
    }
    else
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) != BLOCK_AIR)
            c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        else
            c.setGlobalBlockRelative(local_x, local_y + 1, local_z, getBLOCKWDATA(getBLOCK(block), getBLOCKDATA(block) ^ 1<<7));
    }
}

void DoorRenderer::removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getBLOCKDATA(block) & 1<<7) //If top
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == BLOCK_DOOR)
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, BLOCK_AIR);
    }
    else
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_DOOR)
            c.setGlobalBlockRelative(local_x, local_y + 1, local_z, BLOCK_AIR);
    }
}

const char *DoorRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Door";
}
