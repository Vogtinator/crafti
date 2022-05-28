#include "slabrenderer.h"

constexpr GLFix SlabRenderer::slab_height, SlabRenderer::slab_width;

const TerrainAtlasEntry &SlabRenderer::getSlabTexture(const BLOCK_WDATA block) {
    switch (static_cast<SLAB_TYPE>(getBLOCKDATA(block))) {
        default:
            return terrain_atlas[6][12];
            break;
        case SLAB_STONE:
            return terrain_atlas[1][0];
            break;
        case SLAB_WOOD:
            return terrain_atlas[4][1];
            break;
        case SLAB_PLANKS_NORMAL:
            return terrain_atlas[4][0];
            break;
        case SLAB_WALL:
            return terrain_atlas[7][0];
            break;
        case SLAB_PLANKS_DARK:
            return terrain_atlas[6][12];
            break;
        case SLAB_PLANKS_BRIGHT:
            return terrain_atlas[7][12];
            break;
        case SLAB_COBBLESTONE:
            return terrain_atlas[0][1];
            break;
        case SLAB_NETHERRACK:
            return terrain_atlas[7][6];
            break;
    }
}

const TerrainAtlasEntry &SlabRenderer::destructionTexture(const BLOCK_WDATA block) {
    return getSlabTexture(block);
}

void SlabRenderer::addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) {
    if (true) {//if (getBLOCKDATA(c.getGlobalBlockRelative(local_x, local_y - 1, local_z)) == getBLOCKDATA(block)) {
        c.setLocalBlock(local_x, local_y-1, local_z, BLOCK_WOOD);
        c.setLocalBlock(local_x, local_y, local_z, BLOCK_AIR);
    }
}

void SlabRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{

    const TextureAtlasEntry &slab_top = getSlabTexture(block).current;
    TextureAtlasEntry slab_sid = getSlabTexture(block).current;

    slab_sid.top = slab_sid.top + (slab_sid.bottom - slab_sid.top) * 8 / 16;


    //////
    // GL CODE
    //////

    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x, y, z);

    std::vector<VERTEX> slab_vertices;
    slab_vertices.reserve(20);


    // Slab Side
    slab_vertices.push_back({0, 0, 0, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, 0, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_width, GLFix(0) + slab_height, 0, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_width, 0, 0, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Side
    slab_vertices.push_back({GLFix(0) + slab_width, 0, GLFix(0) - 0 + BLOCK_SIZE, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_width, GLFix(0) + slab_height, GLFix(0) - 0 + BLOCK_SIZE, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, GLFix(0) - 0 + BLOCK_SIZE, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, 0, GLFix(0) - 0 + BLOCK_SIZE, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Back Side
    slab_vertices.push_back({0, 0, GLFix(0) + BLOCK_SIZE, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, 0 + BLOCK_SIZE, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, 0, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, 0, 0, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab (Front) Inside
    slab_vertices.push_back({(GLFix(0) - 0) + slab_width, 0, 0, slab_sid.left, slab_sid.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - 0) + slab_width, GLFix(0) + slab_height, 0, slab_sid.left, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - 0) + slab_width, GLFix(0) + slab_height, 0 + BLOCK_SIZE, slab_sid.right, slab_sid.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({(GLFix(0) - 0) + slab_width, 0, 0 + BLOCK_SIZE, slab_sid.right, slab_sid.bottom, TEXTURE_TRANSPARENT});

    // Slab Top
    slab_vertices.push_back({0, GLFix(0) + slab_height, 0, slab_top.left, slab_top.bottom, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({0, GLFix(0) + slab_height, GLFix(0) + BLOCK_SIZE - 0, slab_top.left, slab_top.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_width - 0, GLFix(0) + slab_height, GLFix(0) + BLOCK_SIZE - 0, slab_top.right, slab_top.top, TEXTURE_TRANSPARENT});
    slab_vertices.push_back({GLFix(0) + slab_width - 0, GLFix(0) + slab_height, 0, slab_top.right, slab_top.bottom, TEXTURE_TRANSPARENT});

    for(auto&& v : slab_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}

void SlabRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    // Render the bottom of the block as a normal side (needs to be replaced later)
    if(side != BLOCK_BOTTOM)
        return;

    // BOTTOM DOESN'T WORK!!! afdghtresdvbhtredfsvcbgfhtresd
    renderNormalBlockSide(local_x, local_y, local_z, side, getSlabTexture(block).current, c);
}

AABB SlabRenderer::getAABB(const BLOCK_WDATA /*block*/, GLFix x, GLFix y, GLFix z)
{
    return {x + 0, y, z + 0, x + 0 + slab_width, y + slab_height, z + 0 + slab_width};
}

void SlabRenderer::drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y)
{
    TextureAtlasEntry tex = getSlabTexture(block).resized;
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

const char *SlabRenderer::getName(const BLOCK_WDATA block)
{
    switch (static_cast<SLAB_TYPE>(getBLOCKDATA(block))) {
        default:
            return "Slab";
            break;
        case SLAB_STONE:
            return "Stone Slab";
            break;
        case SLAB_WOOD:
            return "Wood Slab";
            break;
        case SLAB_PLANKS_NORMAL:
            return "Oak Slab";
            break;
        case SLAB_WALL:
            return "Brick Slab";
            break;
        case SLAB_PLANKS_DARK:
            return "Dark Oak Slab";
            break;
        case SLAB_PLANKS_BRIGHT:
            return "Jungle Slab";
            break;
        case SLAB_COBBLESTONE:
            return "Cobblestone Slab";
            break;
        case SLAB_NETHERRACK:
            return "Netherack Slab";
            break;
    }
}