#include "wirerenderer.h"

void WireRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    renderBillboard((x - c.absX()) / BLOCK_SIZE, (y - c.absY()) / BLOCK_SIZE, (z - c.absZ()) / BLOCK_SIZE, getPOWERSTATE(block) ? terrain_atlas[4][11].current : terrain_atlas[4][10].current, c);
}

void WireRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, const int x, const int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[5][10].resized, true, dest, x, y);
}

void WireRenderer::removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //Removal of a unpowered redstone wire doesn't change anything
    if(getPOWERSTATE(block) == false)
        return;

    //But now there may be different circuits, so check them seperately
    if(getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x - 1, local_y, local_z, c))
        setCircuitState(false, local_x - 1, local_y, local_z, c);

    if(getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x + 1, local_y, local_z, c))
        setCircuitState(false, local_x + 1, local_y, local_z, c);

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x, local_y - 1, local_z, c))
        setCircuitState(false, local_x, local_y - 1, local_z, c);

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x, local_y + 1, local_z, c))
        setCircuitState(false, local_x, local_y + 1, local_z, c);

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x, local_y, local_z - 1, c))
        setCircuitState(false, local_x, local_y, local_z - 1, c);

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(local_x, local_y, local_z + 1, c))
        setCircuitState(false, local_x, local_y, local_z + 1, c);
}

void WireRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //Don't do anything if nothing around is powering
    if(!c.isBlockPowered(local_x, local_y, local_z))
        return;

    //Switch to powering state and become active if directly powered
    if(isDirectlyPowered(local_x, local_y, local_z, c))
    {
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, ACTIVE_BIT, true));
    }
    else
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, 0, true));

    //Now inform the whole redstone chain to become powering
    setCircuitState(true, local_x, local_y, local_z, c);
}

void WireRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    if(getPOWERSTATE(block) == false)
    {
        if(!isDirectlyPowered(local_x, local_y, local_z, c))
            return;

        //Switch to powering state and become active
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, ACTIVE_BIT, true));

        //Now inform the whole redstone chain to become powering
        setCircuitState(true, local_x, local_y, local_z, c);
    }
    else if(getBLOCKDATA(block) == ACTIVE_BIT)
    {
        if(isDirectlyPowered(local_x, local_y, local_z, c))
            return;

        //I'm not active anymore
        c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, 0, true));

        //Check, whether there's any active wire left, if not, turn the whole thing off
        if(!isActiveLeft(local_x, local_y, local_z, c))
            setCircuitState(false, local_x, local_y, local_z, c);
    }
    else
    {
        if(!isDirectlyPowered(local_x, local_y, local_z, c))
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
    c.setGlobalBlockRelative(local_x, local_y, local_z, getBLOCKWDATAPower(block, getBLOCKDATA(block), state));

    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x - 1, local_y, local_z, c);
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x + 1, local_y, local_z, c);
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x, local_y - 1, local_z, c);
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x, local_y + 1, local_z, c);
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x, local_y, local_z - 1, c);
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) != state && getBLOCK(block) == BLOCK_REDSTONE_WIRE)
        setCircuitState(state, local_x, local_y, local_z + 1, c);
}

bool WireRenderer::isDirectlyPowered(const int local_x, const int local_y, const int local_z, Chunk &c)
{
    BLOCK_WDATA block;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;
    if(getPOWERSTATE(block = c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
        return true;

    return false;
}

bool WireRenderer::isActiveLeft(const int local_x, const int local_y, const int local_z, Chunk &c)
{
    BLOCK_WDATA block;

    if(getBLOCKDATA(block = c.getGlobalBlockRelative(local_x, local_y, local_z)) & ACTIVE_BIT)
        return true;

    if(getBLOCKDATA(block) & VISITED_BIT)
        return false;

    //Set this block as visited
    c.setGlobalBlockRelative(local_x, local_y, local_z, block | (VISITED_BIT << 8), false);

    bool ret = false;

    if(getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x - 1, local_y, local_z, c))
        {
            ret = true;
            goto exit;
        }

    if(getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x + 1, local_y, local_z, c))
        {
            ret = true;
            goto exit;
        }

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x, local_y - 1, local_z, c))
        {
            ret = true;
            goto exit;
        }

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x, local_y + 1, local_z, c))
        {
            ret = true;
            goto exit;
        }

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x, local_y, local_z - 1, c))
        {
            ret = true;
            goto exit;
        }

    if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == BLOCK_REDSTONE_WIRE)
        if(isActiveLeft(local_x, local_y, local_z + 1, c))
        {
            ret = true;
            goto exit;
        }

    exit:
    c.setGlobalBlockRelative(local_x, local_y, local_z, block & ~(VISITED_BIT << 8), false);

    return ret;
}
