#include <cstdlib>

#include "stairsrenderer.h"

static constexpr GLFix cake_height = BLOCK_SIZE / 16 * 9;
static constexpr GLFix cake_width = BLOCK_SIZE / 16 * 15;

void StairsRenderer::renderSpecialBlock(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);
    const TextureAtlasEntry &cake_top = terrain_atlas[9][7].current;
    TextureAtlasEntry cake_sid = terrain_atlas[10][7].current;
    cake_sid.top = cake_sid.top + (cake_sid.bottom - cake_sid.top) * 9 / 16;

    c.addUnalignedVertex({x, y, z + cake_offset, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y + cake_height, z + cake_offset, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + cake_height, z + cake_offset, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE, y, z + cake_offset, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    c.addUnalignedVertex({x + BLOCK_SIZE, y, z - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE, y + cake_height, z - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y + cake_height, z - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x, y, z - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    c.addUnalignedVertex({x + cake_offset, y, z + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y, z, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    c.addUnalignedVertex({x - cake_offset + BLOCK_SIZE, y, z, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x - cake_offset + BLOCK_SIZE, y + cake_height, z, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x - cake_offset + BLOCK_SIZE, y + cake_height, z + BLOCK_SIZE, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x - cake_offset + BLOCK_SIZE, y, z + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + cake_offset, cake_top.left, cake_top.bottom, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + cake_offset, y + cake_height, z + BLOCK_SIZE - cake_offset, cake_top.left, cake_top.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE - cake_offset, y + cake_height, z + BLOCK_SIZE - cake_offset, cake_top.right, cake_top.top, TEXTURE_TRANSPARENT});
    c.addUnalignedVertex({x + BLOCK_SIZE - cake_offset, y + cake_height, z + cake_offset, cake_top.right, cake_top.bottom, TEXTURE_TRANSPARENT});
}

AABB StairsRenderer::getAABB(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z)
{
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);

    return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_width, y + cake_height, z + cake_offset + cake_width};
}

void StairsRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[4][0].resized, dest, x, y);
}

const char *StairsRenderer::getName(const BLOCK_WDATA)
{
    return "Stairs";
}
