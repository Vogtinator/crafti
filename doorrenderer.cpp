#include "doorrenderer.h"

constexpr GLFix DoorRenderer::door_depth; //As small as possible, a opened door shouldn't be much of an obstacle
constexpr uint8_t DoorRenderer::DOOR_TOP, DoorRenderer::DOOR_OPEN, DoorRenderer::DOOR_FORCE_OPEN; //FORCE_OPEN: Opened by hand, not redstone

//If the door is open, this array maps the side of the closed door to the side of an open door and vice-versa
static const BLOCK_SIDE door_side_map[] {
        BLOCK_RIGHT,
        BLOCK_LEFT,
        BLOCK_BACK,
        BLOCK_FRONT
};

void DoorRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);
    const TextureAtlasEntry &tex = terrain_atlas[1][(getBLOCKDATA(block) & DOOR_TOP) ? 5 : 6].current;
    const GLFix door_offset = door_depth;

    switch(side)
    {
    case BLOCK_FRONT:
        c.addUnalignedVertex({x, y, z + door_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x, y + BLOCK_SIZE, z + door_offset, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + BLOCK_SIZE, z + door_offset, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z + door_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        break;
    case BLOCK_BACK:
        c.addUnalignedVertex({x + BLOCK_SIZE, y, z - door_offset + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + BLOCK_SIZE, y + BLOCK_SIZE, z - door_offset + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x, y + BLOCK_SIZE, z - door_offset + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x, y, z - door_offset + BLOCK_SIZE, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        break;
    case BLOCK_LEFT:
        c.addUnalignedVertex({x + door_offset, y, z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + door_offset, y + BLOCK_SIZE, z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + door_offset, y + BLOCK_SIZE, z, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x + door_offset, y, z, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        break;
    case BLOCK_RIGHT:
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y, z, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y + BLOCK_SIZE, z, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
        c.addUnalignedVertex({x - door_offset + BLOCK_SIZE, y, z + BLOCK_SIZE, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    default:
        break;
    }
}

AABB DoorRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>((getBLOCKDATA(block) & BLOCK_SIDE_BITS));

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
    drawTexture(*door_preview, dest, 0, 0, 16, 32, x + 4, y, 16, 32);
}

bool DoorRenderer::action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    bool door_open = getBLOCKDATA(block) & DOOR_OPEN;

    toggleState(block, local_x, local_y, local_z, c, door_open ? 0 : (DOOR_OPEN | DOOR_FORCE_OPEN));

    return true;
}

void DoorRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //Only the bottom reacts
    if(getBLOCKDATA(block) & DOOR_TOP)
        return;

    //A manually opened door doesn't change state by itself
    if(getBLOCKDATA(block) & DOOR_FORCE_OPEN)
        return;

    bool redstone_state = c.isBlockPowered(local_x, local_y, local_z)
            || c.isBlockPowered(local_x, local_y + 1, local_z);

    bool door_open = getBLOCKDATA(block) & DOOR_OPEN;

    if(redstone_state == door_open)
        return;

    toggleState(block, local_x, local_y, local_z, c, redstone_state ? DOOR_OPEN : 0);
}

void DoorRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getBLOCKDATA(block) & DOOR_TOP)
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) != BLOCK_AIR)
            c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        else
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, getBLOCKWDATA(getBLOCK(block), getBLOCKDATA(block) ^ DOOR_TOP));
    }
    else
    {
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) != BLOCK_AIR)
            c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
        else
            c.setGlobalBlockRelative(local_x, local_y + 1, local_z, getBLOCKWDATA(getBLOCK(block), getBLOCKDATA(block) ^ DOOR_TOP));
    }
}

void DoorRenderer::removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getBLOCKDATA(block) & DOOR_TOP)
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

PowerState DoorRenderer::powersSide(const BLOCK_WDATA /*block*/, BLOCK_SIDE /*side*/)
{
    return PowerState::NotPowered;
}

const char *DoorRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Door";
}

void DoorRenderer::toggleState(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c, const uint8_t open_state)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);

    uint8_t new_data = door_side_map[side] | open_state | (getBLOCKDATA(block) & DOOR_TOP);

    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATA(getBLOCK(block), new_data));

    //Change the other door part as well
    if(getBLOCKDATA(block) & DOOR_TOP) //If top
            c.setGlobalBlockRelative(local_x, local_y - 1, local_z, getBLOCKWDATA(getBLOCK(block), new_data ^ DOOR_TOP));
    else
            c.setGlobalBlockRelative(local_x, local_y + 1, local_z, getBLOCKWDATA(getBLOCK(block), new_data ^ DOOR_TOP));
}
