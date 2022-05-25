#include "cakerenderer.h"

constexpr GLFix CakeRenderer::cake_height, CakeRenderer::cake_width;

void CakeRenderer::renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    // NOTE: CAKE BOTTOM IS MANAGED LIKE A NORMAL BLOCK


    // Cake offset, the offsettiness of the cake (it isn't a full block ya know)
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);
    const TextureAtlasEntry &cake_top = terrain_atlas[9][7].current;
    TextureAtlasEntry cake_sid = terrain_atlas[10][7].current;
    TextureAtlasEntry cake_inside = terrain_atlas[11][7].current;

    cake_sid.top = cake_sid.top + (cake_sid.bottom - cake_sid.top) * 9 / 16;

    // Size of cake slice
    const GLFix cake_size = cake_width / 2;

    // Cake Back Side
    c.addUnalignedVertex({x, y, z + cake_offset, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y + cake_height, z + cake_offset, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_size, y + cake_height, z + cake_offset, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_size, y, z + cake_offset, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Front Side
    c.addUnalignedVertex({x + cake_size, y, z - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_size, y + cake_height, z - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y + cake_height, z - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y, z - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Right Side
    c.addUnalignedVertex({x + cake_offset, y, z + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y, z, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Left Side
    c.addUnalignedVertex({(x + cake_offset) + cake_size, y, z, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({(x + cake_offset) + cake_size, y + cake_height, z, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({(x + cake_offset) + cake_size, y + cake_height, z + BLOCK_SIZE, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({(x + cake_offset) + cake_size, y, z + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Top
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + cake_offset, cake_top.left, cake_top.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + BLOCK_SIZE - cake_offset, cake_top.left, cake_top.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE - cake_offset, y + cake_height, z + BLOCK_SIZE - cake_offset, cake_top.right, cake_top.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE - cake_offset, y + cake_height, z + cake_offset, cake_top.right, cake_top.bottom, TEXTURE_TRANSPARENT});
}

void CakeRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(side != BLOCK_BOTTOM)
        return;

    renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[12][7].current, c);
}

AABB CakeRenderer::getAABB(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z)
{
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);

    return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_width, y + cake_height, z + cake_offset + cake_width};
}

void CakeRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    TextureAtlasEntry &tex = terrain_atlas[12][8].resized;
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

const char *CakeRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Cake";
}
