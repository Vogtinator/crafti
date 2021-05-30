#include "torchrenderer.h"

void TorchRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry &tex = terrain_atlas[0][5].current;

    renderTorch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)), x, y, z, tex, c);
}

AABB TorchRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    constexpr GLFix torch_width = BLOCK_SIZE / 4;
    constexpr GLFix torch_height = BLOCK_SIZE / 10 * 8;
    constexpr GLFix center = BLOCK_SIZE / 2;

    switch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)))
    {
    default:
    case BLOCK_TOP:
        return {x + center - torch_width, y, z + center - torch_width, x + center + torch_width, y + torch_height, z + center + torch_width};
    case BLOCK_BOTTOM:
        return {x + center - torch_width, y + BLOCK_SIZE - torch_height, z + center - torch_width, x + center + torch_width, y + BLOCK_SIZE, z + center + torch_width};
    case BLOCK_BACK:
    case BLOCK_FRONT:
        return {x + center - torch_width, y, z, x + center + torch_width, y + torch_height, z + BLOCK_SIZE};
    case BLOCK_LEFT:
    case BLOCK_RIGHT:
        return {x, y, z + center - torch_width, x + BLOCK_SIZE, y + torch_height, z + center + torch_width};
    }
}

void TorchRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    TextureAtlasEntry &tex = terrain_atlas[0][5].resized;
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, tex, dest, x, y);
}

const char *TorchRenderer::getName(const BLOCK_WDATA /*block*/)
{
    return "Torch";
}

void TorchRenderer::renderTorch(const BLOCK_SIDE side, const GLFix x, const GLFix y, const GLFix z, TextureAtlasEntry tex, Chunk &c)
{
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x + BLOCK_SIZE/2, y + BLOCK_SIZE/2, z + BLOCK_SIZE/2);

    std::vector<VERTEX> torch_vertices;
    torch_vertices.reserve(8);

    // Draw only the center third of the texture to avoid distortion
    int blockThird = BLOCK_SIZE / 3;
    int texThird = (tex.right - tex.left) / 3;

    tex.left += texThird;
    tex.right -= texThird;

    torch_vertices.push_back({blockThird, 0, BLOCK_SIZE/2, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({blockThird, BLOCK_SIZE, BLOCK_SIZE/2, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({BLOCK_SIZE-blockThird, BLOCK_SIZE, BLOCK_SIZE/2, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({BLOCK_SIZE-blockThird, 0, BLOCK_SIZE/2, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});

    torch_vertices.push_back({BLOCK_SIZE/2, 0, BLOCK_SIZE-blockThird, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, BLOCK_SIZE-blockThird, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, blockThird, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    torch_vertices.push_back({BLOCK_SIZE/2, 0, blockThird, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});

    switch(side)
    {
    case BLOCK_TOP:
        break;
    case BLOCK_BOTTOM:
        nglRotateX(180);
        break;
    case BLOCK_BACK:
        nglRotateX(45);
        break;
    case BLOCK_FRONT:
        nglRotateX(315);
        break;
    case BLOCK_LEFT:
        nglRotateZ(45);
        break;
    case BLOCK_RIGHT:
        nglRotateZ(315);
        break;
    }

    glTranslatef(-BLOCK_SIZE / 2, -BLOCK_SIZE / 2, -BLOCK_SIZE / 2);

    for(auto&& v : torch_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}
