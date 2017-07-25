#include "billboardrenderer.h"

void BillboardRenderer::setEntry(uint8_t data, unsigned int tex_x, unsigned int tex_y, const char *name, const GLFix w, const GLFix h, const GLFix l)
{
    //We halve the sizes as we'd have to do it anyway in getAABB
    map[data] = std::make_tuple(tex_x, tex_y, name, w / 2, h, l / 2);
}

void BillboardRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    auto &bill = map[getBLOCKDATA(block)];
    renderBillboard(x / BLOCK_SIZE, y / BLOCK_SIZE, z / BLOCK_SIZE, terrain_atlas[std::get<0>(bill)][std::get<1>(bill)].current, c);
}

AABB BillboardRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    auto &bill = map[getBLOCKDATA(block)];
    const GLFix center = BLOCK_SIZE/2;
    x += center;
    z += center;

    return {x - std::get<3>(bill), y, z - std::get<5>(bill), x + std::get<3>(bill), y + std::get<4>(bill), z + std::get<5>(bill)};
}

void BillboardRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, const int x, const int y)
{
    auto &bill = map[getBLOCKDATA(block)];
    TextureAtlasEntry &tex = terrain_atlas[std::get<0>(bill)][std::get<1>(bill)].resized;

    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

const char *BillboardRenderer::getName(const BLOCK_WDATA block)
{
    return std::get<2>(map[getBLOCKDATA(block)]);
}
