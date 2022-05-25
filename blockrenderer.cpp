#include <cassert>

#include "blockrenderer.h"

#include "billboardrenderer.h"
#include "cakerenderer.h"
#include "colorrenderer.h"
#include "doorrenderer.h"
#include "fluidrenderer.h"
#include "glassrenderer.h"
#include "lamprenderer.h"
#include "leavesrenderer.h"
#include "pressurerenderer.h"
#include "redtorchrenderer.h"
#include "switchrenderer.h"
#include "tntrenderer.h"
#include "torchrenderer.h"
#include "wheatrenderer.h"
#include "wirerenderer.h"
#include "woolrenderer.h"

UniversalBlockRenderer global_block_renderer;

void BlockRenderer::renderNormalBlockSide(int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry &tex, Chunk &c, const COLOR color)
{
    switch(side)
    {
    case BLOCK_FRONT:
        c.addAlignedVertex(local_x, local_y, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_FRONT_BIT);
        return;
    case BLOCK_BACK:
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.bottom, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.right, tex.top, color);
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.right, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_BACK_BIT);
        return;
    case BLOCK_RIGHT:
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_RIGHT_BIT);
        return;
    case BLOCK_LEFT:
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.left, tex.bottom, color);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.right, tex.top, color);
        c.addAlignedVertex(local_x, local_y, local_z, tex.right, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_LEFT_BIT);
        return;
    case BLOCK_TOP:
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.right, tex.top, color);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_TOP_BIT);
        return;
    case BLOCK_BOTTOM:
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.right, tex.top, color);
        c.addAlignedVertex(local_x, local_y, local_z, tex.right, tex.bottom, color);

        c.setLocalBlockSideRendered(local_x, local_y, local_z, BLOCK_BOTTOM_BIT);
        return;
    default:
        return; //WTF
    }
}

void BlockRenderer::renderNormalBlockSides(int local_x, int local_y, int local_z, int dx, int dy, int dz, const BLOCK_SIDE side, TextureAtlasEntry tex, Chunk &c, const COLOR color)
{
    assert(dx == 1 || dx == 2);
    assert(dy == 1 || dy == 2);
    assert(dz == 1 || dz == 2);

    // tex points to a quad, so anything other then 2x2 needs adjustment
    if(side == BLOCK_BACK || side == BLOCK_FRONT)
    {
        if(dx == 1)
            tex.right -= (tex.right - tex.left) / 2;
        if(dy == 1)
            tex.bottom -= (tex.bottom - tex.top) / 2;
    }
    else if(side == BLOCK_LEFT || side == BLOCK_RIGHT)
    {
        if(dz == 1)
            tex.right -= (tex.right - tex.left) / 2;
        if(dy == 1)
            tex.bottom -= (tex.bottom - tex.top) / 2;
    }
    if(side == BLOCK_BOTTOM || side == BLOCK_TOP)
    {
        if(dx == 1)
            tex.right -= (tex.right - tex.left) / 2;
        if(dz == 1)
            tex.bottom -= (tex.bottom - tex.top) / 2;
    }

    switch(side)
    {
    case BLOCK_FRONT:
        c.addAlignedVertexQuad(local_x, local_y, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertexQuad(local_x, local_y + dy, local_z, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x + dx, local_y + dy, local_z, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x + dx, local_y, local_z, tex.right, tex.bottom, color);
        break;
    case BLOCK_BACK:
        c.addAlignedVertexQuad(local_x + dx, local_y, local_z + 1, tex.left, tex.bottom, color);
        c.addAlignedVertexQuad(local_x + dx, local_y + dy, local_z + 1, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y + dy, local_z + 1, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y, local_z + 1, tex.right, tex.bottom, color);
        break;
    case BLOCK_RIGHT:
        c.addAlignedVertexQuad(local_x + 1, local_y, local_z, tex.right, tex.bottom, color);
        c.addAlignedVertexQuad(local_x + 1, local_y + dy, local_z, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x + 1, local_y + dy, local_z + dz, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x + 1, local_y, local_z + dz, tex.left, tex.bottom, color);
        break;
    case BLOCK_LEFT:
        c.addAlignedVertexQuad(local_x, local_y, local_z + dz, tex.left, tex.bottom, color);
        c.addAlignedVertexQuad(local_x, local_y + dy, local_z + dz, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y + dy, local_z, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y, local_z, tex.right, tex.bottom, color);
        break;
    case BLOCK_TOP:
        c.addAlignedVertexQuad(local_x, local_y + 1, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertexQuad(local_x, local_y + 1, local_z + dz, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x + dx, local_y + 1, local_z + dz, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x + dx, local_y + 1, local_z, tex.right, tex.bottom, color);
        break;
    case BLOCK_BOTTOM:
        c.addAlignedVertexQuad(local_x + dx, local_y, local_z, tex.left, tex.bottom, color);
        c.addAlignedVertexQuad(local_x + dx, local_y, local_z + dz, tex.left, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y, local_z + dz, tex.right, tex.top, color);
        c.addAlignedVertexQuad(local_x, local_y, local_z, tex.right, tex.bottom, color);
        break;
    default:
        return; //WTH
    }

    for(int x = local_x; x < local_x + dx; x++)
        for(int y = local_y; y < local_y + dy; y++)
            for(int z = local_z; z < local_z + dz; z++)
                c.setLocalBlockSideRendered(x, y, z, blockSideToBit(side));
}

void BlockRenderer::renderNormalBlockSidesForceColor(int local_x, int local_y, int local_z, int dx, int dy, int dz, const BLOCK_SIDE side, const COLOR color, Chunk &c)
{
    switch(side)
    {
    case BLOCK_FRONT:
        c.addAlignedVertexForceColor(local_x, local_y, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y + dy, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y + dy, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y, local_z, 0, 0, color);
        break;
    case BLOCK_BACK:
        c.addAlignedVertexForceColor(local_x + dx, local_y, local_z + 1, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y + dy, local_z + 1, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y + dy, local_z + 1, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y, local_z + 1, 0, 0, color);
        break;
    case BLOCK_RIGHT:
        c.addAlignedVertexForceColor(local_x + 1, local_y, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + 1, local_y + dy, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + 1, local_y + dy, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + 1, local_y, local_z + dz, 0, 0, color);
        break;
    case BLOCK_LEFT:
        c.addAlignedVertexForceColor(local_x, local_y, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y + dy, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y + dy, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y, local_z, 0, 0, color);
        break;
    case BLOCK_TOP:
        c.addAlignedVertexForceColor(local_x, local_y + 1, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y + 1, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y + 1, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y + 1, local_z, 0, 0, color);
        break;
    case BLOCK_BOTTOM:
        c.addAlignedVertexForceColor(local_x + dx, local_y, local_z, 0, 0, color);
        c.addAlignedVertexForceColor(local_x + dx, local_y, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y, local_z + dz, 0, 0, color);
        c.addAlignedVertexForceColor(local_x, local_y, local_z, 0, 0, color);
        break;
    default:
        return; //WTF
    }

    for(int x = local_x; x < local_x + dx; x++)
        for(int y = local_y; y < local_y + dy; y++)
            for(int z = local_z; z < local_z + dz; z++)
                c.setLocalBlockSideRendered(x, y, z, blockSideToBit(side));
}

void BlockRenderer::renderNormalConnectedBlockSide(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry &tex, const COLOR col, Chunk &c)
{
    const BLOCK this_block = getBLOCK(block);

    switch(side)
    {
    case BLOCK_TOP:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y + 1, local_z)) == this_block)
            return;
        break;
    case BLOCK_BOTTOM:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == this_block)
            return;
        break;
    case BLOCK_LEFT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x - 1, local_y, local_z)) == this_block)
            return;
        break;
    case BLOCK_RIGHT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x + 1, local_y, local_z)) == this_block)
            return;
        break;
    case BLOCK_BACK:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z + 1)) == this_block)
            return;
        break;
    case BLOCK_FRONT:
        if(getBLOCK(c.getGlobalBlockRelative(local_x, local_y, local_z - 1)) == this_block)
            return;
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tex, c, col);
}

void BlockRenderer::renderBillboard(int local_x, int local_y, int local_z, const TextureAtlasEntry &tex, Chunk &c)
{
    c.addAlignedVertex(local_x, local_y, local_z, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);

    c.addAlignedVertex(local_x, local_y, local_z + 1, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
    c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE);
}

void BlockRenderer::drawTextureAtlasEntry(TEXTURE &src, const TextureAtlasEntry &tex, TEXTURE &dest, const int dest_x, const int dest_y)
{
    drawTexture(src, dest, tex.left, tex.top, tex.right - tex.left, tex.bottom - tex.top, dest_x, dest_y, tex.right - tex.left, tex.bottom - tex.top);
}

UniversalBlockRenderer::UniversalBlockRenderer()
{
    auto color_renderer = std::make_shared<ColorBlockRenderer>();
    auto normal_renderer = std::make_shared<NormalBlockRenderer>();
    auto oriented_renderer = std::make_shared<OrientedBlockRenderer>();
    auto null_renderer = std::make_shared<NullBlockRenderer>();

    BLOCK i = 1;
    for(; i <= BLOCK_NORMAL_LAST; ++i)
        map[i] = normal_renderer;

    while(true)
    {
        map[i] = null_renderer;
        if(i == 255)
            break;

        i++;
    }

    map[BLOCK_AIR] = null_renderer;
    map[BLOCK_BOOKSHELF] = oriented_renderer;
    map[BLOCK_CAKE] = std::make_shared<CakeRenderer>();
    map[BLOCK_CRAFTING_TABLE] = oriented_renderer;
    map[BLOCK_DIRT] = color_renderer;
    map[BLOCK_DOOR] = std::make_shared<DoorRenderer>();
    map[BLOCK_FURNACE] = oriented_renderer;
    map[BLOCK_GLASS] = std::make_shared<GlassRenderer>();
    map[BLOCK_GRASS] = color_renderer;
    map[BLOCK_LEAVES] = std::make_shared<LeavesRenderer>();
    map[BLOCK_TNT] = std::make_shared<TNTRenderer>();
    map[BLOCK_PLANKS_NORMAL] = color_renderer;
    map[BLOCK_PUMPKIN] = oriented_renderer;
    map[BLOCK_REDSTONE_LAMP] = std::make_shared<LampRenderer>();
    map[BLOCK_REDSTONE_SWITCH] = std::make_shared<SwitchRenderer>();
    map[BLOCK_REDSTONE_WIRE] = std::make_shared<WireRenderer>();
    map[BLOCK_REDSTONE_TORCH] = std::make_shared<RedstoneTorchRenderer>();
    map[BLOCK_PRESSURE_PLATE] = std::make_shared<PressurePlateRenderer>();
    map[BLOCK_SAND] = color_renderer;
    map[BLOCK_STONE] = color_renderer;
    map[BLOCK_TORCH] = std::make_shared<TorchRenderer>();
    map[BLOCK_WATER] = std::make_shared<FluidRenderer>(13, 12, "Water");
    map[BLOCK_LAVA] = std::make_shared<FluidRenderer>(13, 14, "Lava");
    map[BLOCK_WHEAT] = std::make_shared<WheatRenderer>();
    map[BLOCK_WOOD] = color_renderer;
    map[BLOCK_WOOL] = std::make_shared<WoolRenderer>();

    auto flower_renderer = std::make_shared<BillboardRenderer>();
    flower_renderer->setEntry(0, 12, 0, "Red flower", BLOCK_SIZE, BLOCK_SIZE/2, BLOCK_SIZE);
    flower_renderer->setEntry(1, 13, 0, "Yellow flower", BLOCK_SIZE, BLOCK_SIZE/2, BLOCK_SIZE);
    map[BLOCK_FLOWER] = flower_renderer;

    auto mushroom_renderer = std::make_shared<BillboardRenderer>();
    mushroom_renderer->setEntry(0, 12, 1, "Red mushroom", BLOCK_SIZE/3, BLOCK_SIZE/3, BLOCK_SIZE/3);
    mushroom_renderer->setEntry(1, 13, 1, "Grey mushroom", BLOCK_SIZE/3, BLOCK_SIZE/3, BLOCK_SIZE/3);
    map[BLOCK_MUSHROOM] = mushroom_renderer;

    auto spiderweb_renderer = std::make_shared<BillboardRenderer>();
    spiderweb_renderer->setEntry(0, 11, 0, "Spiderweb", BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
    map[BLOCK_SPIDERWEB] = spiderweb_renderer;

#ifdef DEBUG
    puts("UniversalBlockRenderer created.");
#endif
}

UniversalBlockRenderer::~UniversalBlockRenderer()
{
#ifdef DEBUG
    puts("UniversalBlockRenderer destroyed.");
#endif
}

void UniversalBlockRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    return map[getBLOCK(block)]->renderSpecialBlock(block, x, y, z, c);
}

void UniversalBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    return map[getBLOCK(block)]->geometryNormalBlock(block, local_x, local_y, local_z, side, c);
}

bool UniversalBlockRenderer::isOpaque(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isOpaque(block);
}

bool UniversalBlockRenderer::isObstacle(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isObstacle(block);
}

bool UniversalBlockRenderer::isOriented(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isOriented(block);
}

bool UniversalBlockRenderer::isFullyOriented(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isFullyOriented(block);
}

bool UniversalBlockRenderer::isBlockShaped(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isBlockShaped(block);
}

AABB UniversalBlockRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    return map[getBLOCK(block)]->getAABB(block, x, y, z);
}

void UniversalBlockRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int x, const int y)
{
    return map[getBLOCK(block)]->drawPreview(block, dest, x, y);
}

const TerrainAtlasEntry &BlockRenderer::materialTexture(const BLOCK_WDATA block) {
    auto block_nr = getBLOCK(block);
    if(block_nr <= BLOCK_NORMAL_LAST)
        return block_textures[block_nr][BLOCK_FRONT];
    else if(block_nr >= BLOCK_SPECIAL_START && block_nr <= BLOCK_SPECIAL_LAST)
        return special_block_textures[block_nr - BLOCK_SPECIAL_START];

    return block_textures[BLOCK_STONE][BLOCK_FRONT];
}

const TerrainAtlasEntry &UniversalBlockRenderer::materialTexture(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->materialTexture(block);
}

bool UniversalBlockRenderer::action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->action(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->tick(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->addedBlock(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::removedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->removedBlock(block, local_x, local_y, local_z, c);
}

PowerState UniversalBlockRenderer::powersSide(const BLOCK_WDATA block, BLOCK_SIDE side)
{
    return map[getBLOCK(block)]->powersSide(block, side);
}

const char *UniversalBlockRenderer::getName(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->getName(block);
}

AABB DumbBlockRenderer::getAABB(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z)
{
    return {x, y, z, x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
}

bool NormalBlockRenderer::shouldRenderFaceAndItsTheSameAs(const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c, const BLOCK_WDATA block)
{
    /* Multiple conditions have to be met:
     * 1. Be in the same Chunk
     * 2. Has to be same block
     * 3. Hasn't been rendered already
     * 4. Not covered by another block */

    if(local_x < 0 || local_y < 0 || local_z < 0 || local_x >= Chunk::SIZE || local_y >= Chunk::SIZE || local_z >= Chunk::SIZE)
        return false;

    if(c.getLocalBlock(local_x, local_y, local_z) != block)
        return false;

    if(c.isLocalBlockSideRendered(local_x, local_y, local_z, blockSideToBit(side)))
        return false;

    switch(side)
    {
    case BLOCK_FRONT:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x, local_y, local_z - 1));
    case BLOCK_BACK:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x, local_y, local_z + 1));
    case BLOCK_LEFT:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x - 1, local_y, local_z));
    case BLOCK_RIGHT:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x + 1, local_y, local_z));
    case BLOCK_TOP:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x, local_y + 1, local_z));
    case BLOCK_BOTTOM:
        return !global_block_renderer.isOpaque(c.getGlobalBlockRelative(local_x, local_y - 1, local_z));
    default:
        return false; //WTF?
    }
}

void NormalBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    //If there isn't a prerendered quad texture for this block, skip it
    if(!quad_block_textures[getBLOCK(block)][side].has_quad)
        return BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, block_textures[getBLOCK(block)][side].current, c);

    switch(side)
    {
    case BLOCK_BACK:
    case BLOCK_FRONT:
    {
        bool can_width = shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block),
             can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block);
        if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y + 1, local_z, side, c, block))
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 2, 2, 1, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_width)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 2, 1, 1, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_height)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 1, 2, 1, side, quad_block_textures[getBLOCK(block)][side].tae, c);

        break;
    }
    case BLOCK_LEFT:
    case BLOCK_RIGHT:
    {
        bool can_width = shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block),
             can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z, side, c, block);
        if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x, local_y + 1, local_z + 1, side, c, block))
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 1, 2, 2, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_width)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 1, 1, 2, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_height)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 1, 2, 1, side, quad_block_textures[getBLOCK(block)][side].tae, c);

        break;
    }
    case BLOCK_BOTTOM:
    case BLOCK_TOP:
    {
        bool can_width = shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z, side, c, block),
             can_height = shouldRenderFaceAndItsTheSameAs(local_x, local_y, local_z + 1, side, c, block);
        if(can_width && can_height && shouldRenderFaceAndItsTheSameAs(local_x + 1, local_y, local_z + 1, side, c, block))
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 2, 1, 2, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_width)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 2, 1, 1, side, quad_block_textures[getBLOCK(block)][side].tae, c);
        else if(can_height)
            return BlockRenderer::renderNormalBlockSides(local_x, local_y, local_z, 1, 1, 2, side, quad_block_textures[getBLOCK(block)][side].tae, c);

        break;
    }
    default:
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, block_textures[getBLOCK(block)][side].current, c);
}

void NormalBlockRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int dest_x, const int dest_y)
{
    const TextureAtlasEntry tex = block_textures[getBLOCK(block)][BLOCK_FRONT].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, dest_x, dest_y);
}

void OrientedBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    BLOCK_SIDE map[BLOCK_SIDE_LAST + 1];

    BLOCK type = getBLOCK(block);
    switch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)))
    {
    default:
    case BLOCK_FRONT:
        map[BLOCK_TOP] = BLOCK_TOP;
        map[BLOCK_BOTTOM] = BLOCK_BOTTOM;
        map[BLOCK_LEFT] = BLOCK_LEFT;
        map[BLOCK_RIGHT] = BLOCK_RIGHT;
        map[BLOCK_BACK] = BLOCK_BACK;
        map[BLOCK_FRONT] = BLOCK_FRONT;
        break;
    case BLOCK_BACK:
        map[BLOCK_TOP] = BLOCK_TOP;
        map[BLOCK_BOTTOM] = BLOCK_BOTTOM;
        map[BLOCK_LEFT] = BLOCK_RIGHT;
        map[BLOCK_RIGHT] = BLOCK_LEFT;
        map[BLOCK_BACK] = BLOCK_FRONT;
        map[BLOCK_FRONT] = BLOCK_BACK;
        break;
    case BLOCK_LEFT:
        map[BLOCK_TOP] = BLOCK_TOP;
        map[BLOCK_BOTTOM] = BLOCK_BOTTOM;
        map[BLOCK_LEFT] = BLOCK_FRONT;
        map[BLOCK_RIGHT] = BLOCK_BACK;
        map[BLOCK_BACK] = BLOCK_LEFT;
        map[BLOCK_FRONT] = BLOCK_RIGHT;
        break;
    case BLOCK_RIGHT:
        map[BLOCK_TOP] = BLOCK_TOP;
        map[BLOCK_BOTTOM] = BLOCK_BOTTOM;
        map[BLOCK_LEFT] = BLOCK_BACK;
        map[BLOCK_RIGHT] = BLOCK_FRONT;
        map[BLOCK_BACK] = BLOCK_RIGHT;
        map[BLOCK_FRONT] = BLOCK_LEFT;
        break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, block_textures[type][map[side]].current, c);
}
