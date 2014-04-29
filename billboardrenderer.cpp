#include "billboardrenderer.h"

void BillboardRenderer::setEntry(uint8_t data, unsigned int tex_x, unsigned int tex_y, const char *name)
{
    map[data] = std::make_tuple(tex_x, tex_y, name);
}

void BillboardRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    auto &bill = map[getBLOCKDATA(block)];
    renderBillboard((x - c.absX()) / BLOCK_SIZE, (y - c.absY()) / BLOCK_SIZE, (z - c.absZ()) / BLOCK_SIZE, terrain_atlas[std::get<0>(bill)][std::get<1>(bill)].current, c);
}

AABB BillboardRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    return {}; //TODO
}

void BillboardRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y)
{
    auto &bill = map[getBLOCKDATA(block)];
    TextureAtlasEntry &tex = terrain_atlas[std::get<0>(bill)][std::get<1>(bill)].resized;
    drawTransparentTexture(*terrain_resized, tex.left, tex.top, dest, x, y, tex.right - tex.left, tex.bottom - tex.top);
}

const char *BillboardRenderer::getName(const BLOCK_WDATA block)
{
    return std::get<2>(map[getBLOCKDATA(block)]);
}
