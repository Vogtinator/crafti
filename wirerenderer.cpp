#include "wirerenderer.h"

struct Pos { int x, y, z; };
struct AdjacentRedstone {
	int count = 0;
	Pos positions[4];
	void add(const Pos &pos) { positions[count++] = pos; };
};

void getAdjacentRedstone(int local_x, int local_y, int local_z, Chunk &c, AdjacentRedstone &ar)
{
	bool go_up = getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == BLOCK_AIR;
	for(auto pos : {Pos{-1, 0, 0}, {1, 0, 0}, {0, 0, -1}, {0, 0, 1}})
	{
		auto x = local_x + pos.x;
		auto y = local_y + pos.y;
		auto z = local_z + pos.z;
		auto this_block = c.getGlobalBlockRelative(x, y, z);
		printf("(%d,%d,%d, %d, %d, %d) ", x, y, z, this_block, c.getGlobalBlockRelative(x, y - 1, z), getBLOCK(c.getGlobalBlockRelative(x, y - 1, z)));
		if(getBLOCK(this_block) == BLOCK_REDSTONE_WIRE)
			ar.add(Pos{x, y + 0, z});
		else if(go_up && global_block_renderer.isObstacle(this_block) && getBLOCK(c.getGlobalBlockRelative(x, y + 1, z)) == BLOCK_REDSTONE_WIRE)
			ar.add(Pos{x, y + 1, z});
		else if(getBLOCK(this_block) == BLOCK_AIR && getBLOCK(c.getGlobalBlockRelative(x, y - 1, z)) == BLOCK_REDSTONE_WIRE)
			ar.add(Pos{x, y - 1, z});
	}
	printf("AR: ");
	for(int i = 0; i < ar.count; ++i)
		printf("(%d,%d,%d) ", ar.positions[i].x, ar.positions[i].y, ar.positions[i].z);
	printf("\n");
}

void WireRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    //renderBillboard(x / BLOCK_SIZE, y / BLOCK_SIZE, z / BLOCK_SIZE, getPOWERSTATE(block) ? terrain_atlas[4][11].current : terrain_atlas[4][10].current, c);
	const TextureAtlasEntry &tex = terrain_atlas[4][getPOWERSTATE(block) ? 11 : 10].current;
	const GLFix height = BLOCK_SIZE / 16;

    c.addUnalignedVertex(x, y + height, z, tex.left, tex.bottom, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(x, y + height, z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(x + BLOCK_SIZE, y + height, z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
    c.addUnalignedVertex(x + BLOCK_SIZE, y + height, z, tex.right, tex.bottom, TEXTURE_DRAW_BACKFACE | TEXTURE_TRANSPARENT);
}

void WireRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, const int x, const int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[5][10].resized, dest, x, y);
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
		if(getBLOCK(c.getGlobalBlockRelative(pos.x, pos.y, pos.z)) == BLOCK_REDSTONE_WIRE && !isActiveLeft(pos.y, pos.y, pos.z, c))
			setCircuitState(false, pos.y, pos.y, pos.z, c);
	}
}

void WireRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    //Don't do anything if nothing around is powering
    if(!c.isBlockPowered(local_x, local_y, local_z))
        return;

    //Switch to powering state and become active if directly powered
    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, isDirectlyPowered(local_x, local_y, local_z, c) ? ACTIVE_BIT : 0, true));

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

	AdjacentRedstone ar;
	getAdjacentRedstone(local_x, local_y, local_z, c, ar);
	for(int i = 0; i < ar.count; ++i)
	{
		Pos &pos = ar.positions[i];
		BLOCK_WDATA block = c.getGlobalBlockRelative(local_x + pos.x, local_y + pos.y, local_z + pos.z);
		if(getPOWERSTATE(block) != state)
			setCircuitState(state, local_x + pos.x, local_y + pos.y, local_z + pos.z, c);
	}
}

bool WireRenderer::isDirectlyPowered(const int local_x, const int local_y, const int local_z, Chunk &c)
{
	for(auto pos : {Pos{-1, 0, 0}, {1, 0, 0}, {0, 0, -1}, {0, 0, 1}, {0, -1, 0}})
	{
		BLOCK_WDATA block = c.getGlobalBlockRelative(local_x + pos.x, local_y + pos.y, local_z + pos.z);
		if(getPOWERSTATE(block) == true && getBLOCK(block) != BLOCK_REDSTONE_WIRE)
			return true;
	}

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

	AdjacentRedstone ar;
	getAdjacentRedstone(local_x, local_y, local_z, c, ar);
	for(int i = 0; i < ar.count; ++i)
	{
		Pos &pos = ar.positions[i];
		if(getBLOCK(c.getGlobalBlockRelative(local_x + pos.x, local_y + pos.y, local_z + pos.z)) == BLOCK_REDSTONE_WIRE
		   && isActiveLeft(local_x + pos.x, local_y + pos.y, local_z + pos.z, c))
		{
            ret = true;
            break;
		}
	}
    c.setGlobalBlockRelative(local_x, local_y, local_z, block & ~(VISITED_BIT << 8), false);

    return ret;
}
