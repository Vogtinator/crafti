#include "blockrenderer.h"

#include "billboardrenderer.h"
#include "cakerenderer.h"

UniversalBlockRenderer globalBlockRenderer;

void BlockRenderer::renderNormalBlockSide(int local_x, int local_y, int local_z, const BLOCK_SIDE side, const TextureAtlasEntry tex, Chunk &c)
{
    switch(side)
    {
    case BLOCK_FRONT:
        c.addAlignedVertex(local_x, local_y, local_z, tex.left, tex.bottom, 0);
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, 0);
        return;
    case BLOCK_BACK:
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.bottom, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.right, tex.bottom, 0);
        return;
    case BLOCK_RIGHT:
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.bottom, 0);
        return;
    case BLOCK_LEFT:
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.left, tex.bottom, 0);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x, local_y, local_z, tex.right, tex.bottom, 0);
        return;
    case BLOCK_TOP:
        c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.bottom, 0);
        c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.bottom, 0);
        return;
    case BLOCK_BOTTOM:
        c.addAlignedVertex(local_x + 1, local_y, local_z, tex.left, tex.bottom, 0);
        c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.left, tex.top, 0);
        c.addAlignedVertex(local_x, local_y, local_z + 1, tex.right, tex.top, 0);
        c.addAlignedVertex(local_x, local_y, local_z, tex.right, tex.bottom, 0);
        return;
    default:
        return; //WTF
    }
}

void BlockRenderer::renderBillboard(int local_x, int local_y, int local_z, const TextureAtlasEntry tex, Chunk &c)
{
    //0xFFFF: Black = transparent and no backface culling
    c.addAlignedVertex(local_x, local_y, local_z, tex.left, tex.bottom, 0xFFFF);
    c.addAlignedVertex(local_x, local_y + 1, local_z, tex.left, tex.top, 0xFFFF);
    c.addAlignedVertex(local_x + 1, local_y + 1, local_z + 1, tex.right, tex.top, 0xFFFF);
    c.addAlignedVertex(local_x + 1, local_y, local_z + 1, tex.right, tex.bottom, 0xFFFF);

    c.addAlignedVertex(local_x, local_y, local_z + 1, tex.left, tex.bottom, 0xFFFF);
    c.addAlignedVertex(local_x, local_y + 1, local_z + 1, tex.left, tex.top, 0xFFFF);
    c.addAlignedVertex(local_x + 1, local_y + 1, local_z, tex.right, tex.top, 0xFFFF);
    c.addAlignedVertex(local_x + 1, local_y, local_z, tex.right, tex.bottom, 0xFFFF);
}

UniversalBlockRenderer::UniversalBlockRenderer()
{
    auto normal_renderer = std::make_shared<NormalBlockRenderer>();
    auto null_renderer = std::make_shared<NullBlockRenderer>();
    BLOCK i = 1;
    for(; i < BLOCK_NORMAL_MAX; i++)
        map[i] = normal_renderer;

    while(true)
    {
        map[i] = null_renderer;
        if(i == 255)
            break;

        i++;
    }

    map[BLOCK_AIR] = null_renderer;
    map[BLOCK_CAKE] = std::make_shared<CakeRenderer>();

    auto flower_renderer = std::make_shared<BillboardRenderer>();
    flower_renderer->setEntry(0, 12, 0, "Red flower");
    flower_renderer->setEntry(1, 13, 0, "Yellow flower");
    map[BLOCK_FLOWER] = flower_renderer;

    auto mushroom_renderer = std::make_shared<BillboardRenderer>();
    mushroom_renderer->setEntry(0, 12, 1, "Red mushroom");
    mushroom_renderer->setEntry(1, 13, 1, "Grey mushroom");
    map[BLOCK_MUSHROOM] = mushroom_renderer;

    auto spiderweb_renderer = std::make_shared<BillboardRenderer>();
    spiderweb_renderer->setEntry(0, 11, 0, "Spiderweb");
    map[BLOCK_SPIDERWEB] = spiderweb_renderer;
}

void UniversalBlockRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    return map[getBLOCK(block)]->renderSpecialBlock(block, x, y, z, c);
}

int UniversalBlockRenderer::indicesNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c)
{
    return map[getBLOCK(block)]->indicesNormalBlock(block, local_x, local_y, local_z, side, c);
}

void UniversalBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c)
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

bool UniversalBlockRenderer::isBlockShaped(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->isBlockShaped(block);
}

AABB UniversalBlockRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    return map[getBLOCK(block)]->getAABB(block, x, y, z);
}

void UniversalBlockRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y)
{
    return map[getBLOCK(block)]->drawPreview(block, dest, x, y);
}

void UniversalBlockRenderer::tick(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->tick(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::updateBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->updateBlock(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::addBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->addBlock(block, local_x, local_y, local_z, c);
}

void UniversalBlockRenderer::removeBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c)
{
    return map[getBLOCK(block)]->removeBlock(block, local_x, local_y, local_z, c);
}

const char *UniversalBlockRenderer::getName(const BLOCK_WDATA block)
{
    return map[getBLOCK(block)]->getName(block);
}

void NormalBlockRenderer::geometryNormalBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, const BLOCK_SIDE side, Chunk &c)
{
    return BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, block_textures[getBLOCK(block)][side].current, c);
}

void NormalBlockRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int dest_x, int dest_y)
{
    const TextureAtlasEntry tex = block_textures[getBLOCK(block)][BLOCK_FRONT].resized;
    return drawTexture(*terrain_resized, tex.left, tex.top, dest, dest_x, dest_y, tex.right - tex.left, tex.bottom - tex.top);
}
