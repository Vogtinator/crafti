#include "cakerenderer.h"

constexpr GLFix CakeRenderer::cake_height, CakeRenderer::cake_width;

void CakeRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    // NOTE: CAKE BOTTOM IS MANAGED LIKE A NORMAL BLOCK


    // Cake offset, the offsettiness of the cake (it isn't a full block ya know)
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);
    const TextureAtlasEntry &cake_top = terrain_atlas[9][7].current;
    TextureAtlasEntry cake_sid = terrain_atlas[10][7].current;
    TextureAtlasEntry cake_inside = terrain_atlas[11][7].current;

    cake_sid.top = cake_sid.top + (cake_sid.bottom - cake_sid.top) * 9 / 16;
    cake_inside.top = cake_inside.top + (cake_inside.bottom - cake_inside.top) * 9 / 16;

    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);
    uint8_t cake_remaining = static_cast<uint8_t>(getBLOCKDATA(block));

    // Size of cake slice
    const GLFix cake_size = cake_width / 2;

    //////
    // GL CODE
    //////
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x + BLOCK_SIZE/2, y + BLOCK_SIZE/2, z + BLOCK_SIZE/2);

    std::vector<VERTEX> cake_vertices;
    cake_vertices.reserve(20);

    // Cake Back Side
    cake_vertices.push_back({0, 0, GLFix(0) + cake_offset, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({0, GLFix(0) + cake_height, GLFix(0) + cake_offset, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_size, GLFix(0) + cake_height, GLFix(0) + cake_offset, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_size, 0, GLFix(0) + cake_offset, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Front Side
    cake_vertices.push_back({GLFix(0) + cake_size, 0, GLFix(0) - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_size, GLFix(0) + cake_height, GLFix(0) - cake_offset + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({0, GLFix(0) + cake_height, GLFix(0) - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({0, 0, GLFix(0) - cake_offset + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Right Side
    cake_vertices.push_back({GLFix(0) + cake_offset, 0, GLFix(0) + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_offset, GLFix(0) + cake_height, 0 + BLOCK_SIZE, cake_sid.left, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_offset, GLFix(0) + cake_height, 0, cake_sid.right, cake_sid.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_offset, 0, 0, cake_sid.right, cake_sid.bottom, TEXTURE_TRANSPARENT});

    // Cake Left Side
    cake_vertices.push_back({(GLFix(0) - cake_offset) + cake_size, 0, 0, cake_inside.left, cake_inside.bottom, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({(GLFix(0) - cake_offset) + cake_size, GLFix(0) + cake_height, 0, cake_inside.left, cake_inside.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({(GLFix(0) - cake_offset) + cake_size, GLFix(0) + cake_height, 0 + BLOCK_SIZE, cake_inside.right, cake_inside.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({(GLFix(0) - cake_offset) + cake_size, 0, 0 + BLOCK_SIZE, cake_inside.right, cake_inside.bottom, TEXTURE_TRANSPARENT});

    // Cake Top
    cake_vertices.push_back({GLFix(0) + cake_offset, GLFix(0) + cake_height, GLFix(0) + cake_offset, cake_top.left, cake_top.bottom, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_offset, GLFix(0) + cake_height, GLFix(0) + BLOCK_SIZE - cake_offset, cake_top.left, cake_top.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_size - cake_offset, GLFix(0) + cake_height, GLFix(0) + BLOCK_SIZE - cake_offset, cake_top.right, cake_top.top, TEXTURE_TRANSPARENT});
    cake_vertices.push_back({GLFix(0) + cake_size - cake_offset, GLFix(0) + cake_height, GLFix(0) + cake_offset, cake_top.right, cake_top.bottom, TEXTURE_TRANSPARENT});


    switch(side)
    {
        default:
            break;
        case BLOCK_BACK:
            nglRotateY(270);
            break;
        case BLOCK_FRONT:
            nglRotateY(90);
            break;
        case BLOCK_LEFT:
            nglRotateY(180);
            break;
        case BLOCK_RIGHT:
            nglRotateY(0);
            break;
    }

    glTranslatef(-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2);

    for(auto&& v : cake_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}

void CakeRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    if(side != BLOCK_BOTTOM)
        return;

    renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[12][7].current, c);
}

AABB CakeRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);

    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) * GLFix(0.5f);
    // Size of cake slice
    const GLFix cake_size = cake_width / 2;

    switch(side)
    {
        default:
            return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_size, y + cake_height, z + cake_offset + cake_width};
            break;
        case BLOCK_BACK:
            return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_size, y + cake_height, z + cake_offset + cake_width};
            break;
        case BLOCK_FRONT:
            return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_size, y + cake_height, z + cake_offset + cake_width};
            break;
        case BLOCK_LEFT:
            return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_size, y + cake_height, z + cake_offset + cake_width};
            break;
        case BLOCK_RIGHT: // LEFT X SIDE IS FACING YOU
            return {x + cake_offset, y, z + cake_offset, x + cake_offset + cake_size, y + cake_height, z + cake_offset + cake_width};
            break;
    }
    
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
