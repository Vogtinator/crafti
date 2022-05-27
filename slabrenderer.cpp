#include "slabrenderer.h"

constexpr GLFix SlabRenderer::slab_height, SlabRenderer::slab_width;

void SlabRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    // NOTE: CAKE BOTTOM IS MANAGED LIKE A NORMAL BLOCK


    // Slab offset, the offsettiness of the slab (it isn't a full block ya know)
    const GLFix slab_offset = 0;
    const TextureAtlasEntry &slab_top = terrain_atlas[4][0].current;
    TextureAtlasEntry slab_sid = terrain_atlas[4][0].current;


    slab_sid.top = slab_sid.top + (slab_sid.bottom - slab_sid.top) * 8 / 16;
    
    // Calculate the slab's size
    const GLFix slab_size = slab_width;


    //////
    // GL CODE
    //////
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x + BLOCK_SIZE/2, y + BLOCK_SIZE/2, z + BLOCK_SIZE/2);

    std::vector<VERTEX> slab_vertices;
    slab_vertices.reserve(20);

    //slab_sid.right = slab_sid.right - (slab_sid.right - slab_sid.left) * (slab_max_bites - slab_bites) / 6;

    GLFix slab_left_texturemap = slab_sid.left;
    GLFix slab_right_texturemap = slab_sid.right;


    GLFix slab_top_texturemap = slab_top.right;


    // Slab Side
    slab_vertices.push_back({0, 0, GLFix(0) + slab_offset, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, GLFix(0) + slab_offset, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_size, GLFix(0) + slab_height, GLFix(0) + slab_offset, slab_right_texturemap, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_size, 0, GLFix(0) + slab_offset, slab_right_texturemap, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Side
    slab_vertices.push_back({GLFix(0) + slab_size, 0, GLFix(0) - slab_offset + BLOCK_SIZE, slab_left_texturemap, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_size, GLFix(0) + slab_height, GLFix(0) - slab_offset + BLOCK_SIZE, slab_left_texturemap, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, GLFix(0) - slab_offset + BLOCK_SIZE, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, 0, GLFix(0) - slab_offset + BLOCK_SIZE, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Back Side
    slab_vertices.push_back({GLFix(0) + slab_offset, 0, GLFix(0) + BLOCK_SIZE, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_offset, GLFix(0) + slab_height, 0 + BLOCK_SIZE, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_offset, GLFix(0) + slab_height, 0, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_offset, 0, 0, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab (Front) Inside
    slab_vertices.push_back({(GLFix(0) - slab_offset) + slab_size, 0, 0, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - slab_offset) + slab_size, GLFix(0) + slab_height, 0, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - slab_offset) + slab_size, GLFix(0) + slab_height, 0 + BLOCK_SIZE, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - slab_offset) + slab_size, 0, 0 + BLOCK_SIZE, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Top
    slab_vertices.push_back({GLFix(0) + slab_offset, GLFix(0) + slab_height, GLFix(0) + slab_offset, slab_top.left, slab_top.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_offset, GLFix(0) + slab_height, GLFix(0) + BLOCK_SIZE - slab_offset, slab_top.left, slab_top.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_size - slab_offset, GLFix(0) + slab_height, GLFix(0) + BLOCK_SIZE - slab_offset, slab_top_texturemap, slab_top.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_size - slab_offset, GLFix(0) + slab_height, GLFix(0) + slab_offset, slab_top_texturemap, slab_top.bottom, TEXTURE_TRANSPARENT});

    // Rotate Slab According To Face
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);


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

    for(auto&& v : slab_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}

void SlabRenderer::geometryNormalBlock(const BLOCK_WDATA /*block*/, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    // Render the bottom of the block as a normal side (needs to be replaced later)
    if(side != BLOCK_BOTTOM)
        return;

    // BOTTOM DOESN'T WORK!!! afdghtresdvbhtredfsvcbgfhtresd
    renderNormalBlockSide(local_x, local_y, local_z, side, terrain_atlas[12][7].current, c);
}

AABB SlabRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    // Get block side
    BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block) & BLOCK_SIDE_BITS);
    const GLFix slab_offset = 0;

    // Calculate the slab's size
    const GLFix slab_size = slab_width;

    switch(side)
    {
        default:
            return {x + slab_offset, y, z + slab_offset + (slab_width - slab_size), x + slab_offset + slab_width, y + slab_height, z + slab_offset + slab_width};
            break;
        case BLOCK_BACK:
            return {x + slab_offset, y, z + slab_offset, x + slab_offset + slab_width, y + slab_height, z + slab_offset +  slab_size};
            break;
        case BLOCK_FRONT:
            return {x + slab_offset, y, z + slab_offset + (slab_width - slab_size), x + slab_offset + slab_width, y + slab_height, z + slab_offset + slab_width};
            break;
        case BLOCK_LEFT:
            return {x + slab_offset + (slab_width - slab_size), y, z + slab_offset, x + slab_offset + slab_width, y + slab_height, z + slab_offset + slab_width};
            break;
        case BLOCK_RIGHT:
            return {x + slab_offset, y, z + slab_offset, x + slab_offset + slab_size, y + slab_height, z + slab_offset + slab_width};
            break;
    }
}

void SlabRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    TextureAtlasEntry &tex = terrain_atlas[4][0].resized;
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

const char *SlabRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Slab";
}