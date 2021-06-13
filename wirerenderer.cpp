#include "wirerenderer.h"

/* How Redstone in crafti is implemented:
 *
 */

constexpr GLFix WireRenderer::height;

struct Pos { int x, y, z; };
struct AdjacentRedstone {
    int count = 0;
    Pos positions[4];
    void add(const Pos &pos) { positions[count++] = pos; };
};

static void getAdjacentRedstone(int local_x, int local_y, int local_z, Chunk &c, AdjacentRedstone &ar)
{
    bool can_go_up = getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_AIR;
    for(auto pos : {Pos{-1, 0, 0}, {1, 0, 0}, {0, 0, -1}, {0, 0, 1}})
    {
        auto x = local_x + pos.x;
        auto y = local_y + pos.y;
        auto z = local_z + pos.z;
        auto this_block = c.getGlobalBlockRelative(x, y, z);
        if(getBLOCK(this_block) == BLOCK_REDSTONE_WIRE)
            ar.add(Pos{x, y + 0, z});
        else if(can_go_up && global_block_renderer.isObstacle(this_block) && getBLOCK(c.getGlobalBlockRelative(x, y + 1, z)) == BLOCK_REDSTONE_WIRE)
            ar.add(Pos{x, y + 1, z});
        else if(getBLOCK(this_block) == BLOCK_AIR && getBLOCK(c.getGlobalBlockRelative(x, y - 1, z)) == BLOCK_REDSTONE_WIRE)
            ar.add(Pos{x, y - 1, z});
    }
}

void WireRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry tex = terrain_atlas[4][getPOWERSTATE(block) ? 11 : 10].current;

    // Whether there is a connection in that direction
    bool c_left = false, c_right = false, c_back = false, c_front = false,
         c_left_up = false, c_right_up = false, c_back_up = false, c_front_up = false;

    AdjacentRedstone ar;
    int local_x = x / BLOCK_SIZE, local_y = y / BLOCK_SIZE, local_z = z / BLOCK_SIZE;
    getAdjacentRedstone(local_x, local_y, local_z, c, ar);
    for(int i = 0; i < ar.count; ++i)
    {
        Pos &pos = ar.positions[i];
        if(pos.x == local_x - 1)
        {
            c_left = true;
            if(pos.y == local_y + 1)
                c_left_up = true;
        }
        else if(pos.x == local_x + 1)
        {
            c_right = true;
            if(pos.y == local_y + 1)
                c_right_up = true;
        }
        else if(pos.z == local_z - 1)
        {
            c_back = true;
            if(pos.y == local_y + 1)
                c_back_up = true;
        }
        else if(pos.z == local_z + 1)
        {
            c_front = true;
            if(pos.y == local_y + 1)
                c_front_up = true;
        }
    }

    // Remove a third of the visible redstone when there's no connection in that direction
    GLFix xstart = x, xend = x + BLOCK_SIZE,
          zstart = z, zend = z + BLOCK_SIZE;

    int thirdTex = (tex.right - tex.left) / 3;
    GLFix thirdBlock = BLOCK_SIZE / 3;

    if(!c_left)
    {
        tex.left += thirdTex;
        xstart += thirdBlock;
    }
    if(!c_right)
    {
        tex.right -= thirdTex;
        xend -= thirdBlock;
    }
    if(!c_back)
    {
        tex.bottom -= thirdTex;
        zstart += thirdBlock;
    }
    if(!c_front)
    {
        tex.top += thirdTex;
        zend -= thirdBlock;
    }

    c.addUnalignedVertex(xstart, y + height, zstart, tex.left, tex.bottom, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(xstart, y + height, zend, tex.left, tex.top, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(xend, y + height, zend, tex.right, tex.top, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(xend, y + height, zstart, tex.right, tex.bottom, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
}

AABB WireRenderer::getAABB(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z)
{
    return {x, y, z, x + BLOCK_SIZE, y + height, z + BLOCK_SIZE};
}

void WireRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, const int x, const int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[4][11].resized, dest, x, y);
}

void WireRenderer::removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //Removal of a unpowered redstone wire doesn't change anything
    if(getPOWERSTATE(block) == false)
        return;

    //But now there may be different circuits, so check them seperately
    AdjacentRedstone ar;
    getAdjacentRedstone(local_x, local_y, local_z, c, ar);
    for(int i = 0; i < ar.count; ++i)
    {
        Pos &pos = ar.positions[i];
        if(!isActiveLeft(pos.x, pos.y, pos.z, c))
            setCircuitState(false, pos.x, pos.y, pos.z, c);
    }
}

void WireRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(c.isBlockPowered(local_x, local_y, local_z, true)) //Directly powered?
    {
        //Switch to powering state and become active
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, ACTIVE_BIT, true));

        //Now inform the whole redstone chain to become powering
        setCircuitState(true, local_x, local_y, local_z, c);
    }
    else if(isActiveLeft(local_x, local_y, local_z, c)) // Any of the connected redstone active?
    {
        //Now inform the whole redstone chain to become powering
        setCircuitState(true, local_x, local_y, local_z, c);
    }
}

PowerState WireRenderer::powersSide(const BLOCK_WDATA block, BLOCK_SIDE side)
{
    if(!getPOWERSTATE(block) || side == BLOCK_TOP)
        return PowerState::NotPowered;

    return PowerState::StronglyPowered;
}

void WireRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getPOWERSTATE(block) == false)
    {
        //Directly powered?
        if(!c.isBlockPowered(local_x, local_y, local_z, true))
            return;

        //Switch to powering state and become active
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, ACTIVE_BIT, true));

        //Now inform the whole redstone chain to become powering
        setCircuitState(true, local_x, local_y, local_z, c);
    }
    else if(getBLOCKDATA(block) == ACTIVE_BIT)
    {
        if(c.isBlockPowered(local_x, local_y, local_z, true))
            return;

        //Still powering, but not active
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, 0, true));

        //Check, whether there's any active wire left, if not, turn the whole thing off
        if(!isActiveLeft(local_x, local_y, local_z, c))
            setCircuitState(false, local_x, local_y, local_z, c);
    }
    else
    {
        if(!c.isBlockPowered(local_x, local_y, local_z, true))
            return;

        //Become active
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, ACTIVE_BIT, true));
    }
}

const char *WireRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Redstone Wire";
}

//Sets the power state of the whole circuit, recursively.
//Doesn't touch the BLOCK_DATA
void WireRenderer::setCircuitState(const bool state, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    BLOCK_WDATA block = c.getGlobalBlockRelative(local_x, local_y, local_z);
    block = getBLOCKWDATAPower(block, getBLOCKDATA(block), state);
    c.setGlobalBlockRelative(local_x, local_y, local_z, block);

    AdjacentRedstone ar;
    getAdjacentRedstone(local_x, local_y, local_z, c, ar);
    for(int i = 0; i < ar.count; ++i)
    {
        Pos &pos = ar.positions[i];
        BLOCK_WDATA block = c.getGlobalBlockRelative(pos.x, pos.y, pos.z);
        if(getPOWERSTATE(block) != state)
            setCircuitState(state, pos.x, pos.y, pos.z, c);
    }
}

// Whether the circut at that position has any wire with ACTIVE_BIT set.
bool WireRenderer::isActiveLeft(const int local_x, const int local_y, const int local_z, Chunk &c)
{
    BLOCK_WDATA block = c.getGlobalBlockRelative(local_x, local_y, local_z);

    if(getBLOCKDATA(block) & ACTIVE_BIT)
        return true;

    if(getBLOCKDATA(block) & VISITED_BIT)
        return false;

    //Set this block as visited
    c.setGlobalBlockRelative(local_x, local_y, local_z, block | (VISITED_BIT << 8), false);

    bool ret = false;

    AdjacentRedstone ar;
    getAdjacentRedstone(local_x, local_y, local_z, c, ar);
    for(int i = 0; i < ar.count; ++i)
    {
        Pos &pos = ar.positions[i];
        if(isActiveLeft(pos.x, pos.y, pos.z, c))
        {
            ret = true;
            break;
        }
    }
    c.setGlobalBlockRelative(local_x, local_y, local_z, block & ~(VISITED_BIT << 8), false);

    return ret;
}
