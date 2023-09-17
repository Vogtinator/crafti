#include "switchrenderer.h"

#include "lamprenderer.h"

void SwitchRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry tex = getPOWERSTATE(block) ? terrain_atlas[10][15].current : terrain_atlas[0][6].current;

    glPushMatrix();
    glLoadIdentity();

    glTranslatef(x + BLOCK_SIZE/2, y + BLOCK_SIZE/2, z + BLOCK_SIZE/2);

    std::vector<VERTEX> switch_vertices;
    switch_vertices.reserve(6*4);

    const BLOCK_SIDE side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block));

    switch(side)
    {
    default:
    case BLOCK_TOP:
        break;
    case BLOCK_BOTTOM:
        nglRotateX(180);
        break;
    case BLOCK_BACK:
        nglRotateX(90);
        break;
    case BLOCK_FRONT:
        nglRotateX(270);
        break;
    case BLOCK_LEFT:
        nglRotateZ(90);
        nglRotateY(90);
        break;
    case BLOCK_RIGHT:
        nglRotateZ(270);
        nglRotateY(90);
        break;
    }

    glTranslatef(0, -BLOCK_SIZE/2, 0);

    // Draw the block
    const GLFix block_width = 0.3f, block_length = 0.6f, block_height = 0.2f;
    TextureAtlasEntry stone_tb = terrain_atlas[0][1].current;
    stone_tb.bottom = stone_tb.top + int(block_length * (stone_tb.bottom - stone_tb.top));
    stone_tb.right = stone_tb.left + int(block_width * (stone_tb.right - stone_tb.left));
    TextureAtlasEntry stone_lr = terrain_atlas[0][1].current;
    stone_lr.bottom = stone_lr.top + int(block_length * (stone_lr.bottom - stone_lr.top));
    stone_lr.right = stone_lr.left + int(block_height * (stone_lr.right - stone_lr.left));
    TextureAtlasEntry stone_fb = terrain_atlas[0][1].current;
    stone_fb.bottom = stone_fb.top + int(block_height * (stone_fb.bottom - stone_fb.top));
    stone_fb.right = stone_fb.left + int(block_width * (stone_fb.right - stone_fb.left));

    const GLFix left = GLFix(-BLOCK_SIZE/2) + (GLFix(BLOCK_SIZE) - block_width*BLOCK_SIZE) / 2,
                right = left + block_width*BLOCK_SIZE,
                bottom = 0,
                top = bottom + block_height*BLOCK_SIZE,
                front = GLFix(-BLOCK_SIZE/2) + (GLFix(BLOCK_SIZE) - block_length*BLOCK_SIZE) / 2,
                back = front + block_length*BLOCK_SIZE;

    // Top
    switch_vertices.push_back({left, top, front, stone_tb.left, stone_tb.bottom, 0});
    switch_vertices.push_back({left, top, back, stone_tb.left, stone_tb.top, 0});
    switch_vertices.push_back({right, top, back, stone_tb.right, stone_tb.top, 0});
    switch_vertices.push_back({right, top, front, stone_tb.right, stone_tb.bottom, 0});

    // Bottom
    switch_vertices.push_back({left, bottom, back, stone_tb.left, stone_tb.bottom, 0});
    switch_vertices.push_back({left, bottom, front, stone_tb.left, stone_tb.top, 0});
    switch_vertices.push_back({right, bottom, front, stone_tb.right, stone_tb.top, 0});
    switch_vertices.push_back({right, bottom, back, stone_tb.right, stone_tb.bottom, 0});

    // Front
    switch_vertices.push_back({left, bottom, front, stone_fb.left, stone_fb.bottom, 0});
    switch_vertices.push_back({left, top, front, stone_fb.left, stone_fb.top, 0});
    switch_vertices.push_back({right, top, front, stone_fb.right, stone_fb.top, 0});
    switch_vertices.push_back({right, bottom, front, stone_fb.right, stone_fb.bottom, 0});

    // Back
    switch_vertices.push_back({right, bottom, back, stone_fb.left, stone_fb.bottom, 0});
    switch_vertices.push_back({right, top, back, stone_fb.left, stone_fb.top, 0});
    switch_vertices.push_back({left, top, back, stone_fb.right, stone_fb.top, 0});
    switch_vertices.push_back({left, bottom, back, stone_fb.right, stone_fb.bottom, 0});

    // Left
    switch_vertices.push_back({left, bottom, back, stone_lr.left, stone_lr.bottom, 0});
    switch_vertices.push_back({left, top, back, stone_lr.left, stone_lr.top, 0});
    switch_vertices.push_back({left, top, front, stone_lr.right, stone_lr.top, 0});
    switch_vertices.push_back({left, bottom, front, stone_lr.right, stone_lr.bottom, 0});

    // Right
    switch_vertices.push_back({right, bottom, front, stone_lr.left, stone_lr.bottom, 0});
    switch_vertices.push_back({right, top, front, stone_lr.left, stone_lr.top, 0});
    switch_vertices.push_back({right, top, back, stone_lr.right, stone_lr.top, 0});
    switch_vertices.push_back({right, bottom, back, stone_lr.right, stone_lr.bottom, 0});

    for(auto&& v : switch_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }
    switch_vertices.clear();

    glTranslatef(0, block_height/2 * BLOCK_SIZE, 0);

    // Draw only the center third of the texture to avoid distortion
    int blockThird = BLOCK_SIZE / 3;
    int texThird = (tex.right - tex.left) / 3;

    tex.left += texThird;
    tex.right -= texThird;

    switch_vertices.push_back({blockThird, 0, BLOCK_SIZE/2, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({blockThird, BLOCK_SIZE - BLOCK_SIZE/4, BLOCK_SIZE/2, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({BLOCK_SIZE-blockThird, BLOCK_SIZE - BLOCK_SIZE/4, BLOCK_SIZE/2, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({BLOCK_SIZE-blockThird, 0, BLOCK_SIZE/2, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});

    switch_vertices.push_back({BLOCK_SIZE/2, 0, BLOCK_SIZE-blockThird, tex.left, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE-BLOCK_SIZE/4, BLOCK_SIZE-blockThird, tex.left, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE-BLOCK_SIZE/4, blockThird, tex.right, tex.top, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});
    switch_vertices.push_back({BLOCK_SIZE/2, 0, blockThird, tex.right, tex.bottom, TEXTURE_TRANSPARENT | TEXTURE_DRAW_BACKFACE});

    if(getPOWERSTATE(block))
        nglRotateX(40);
    else
        nglRotateX(320);

    glTranslatef(-BLOCK_SIZE / 2, 0, -BLOCK_SIZE / 2);

    for(auto&& v : switch_vertices)
    {
        VERTEX v1;
        nglMultMatVectRes(transformation, &v, &v1);
        c.addUnalignedVertex(v1.x, v1.y, v1.z, v.u, v.v, v.c);
    }

    glPopMatrix();
}

void SwitchRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    return BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[0][6].resized, dest, x, y);
}

bool SwitchRenderer::action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c)
{
    c.setLocalBlock(local_x, local_y, local_z, getBLOCKWDATAPower(block, getBLOCKDATA(block), !getPOWERSTATE(block)));

    return true;
}

PowerState SwitchRenderer::powersSide(const BLOCK_WDATA block, BLOCK_SIDE side)
{
    if(!getPOWERSTATE(block))
        return PowerState::NotPowered;

    const BLOCK_SIDE attached_side = static_cast<BLOCK_SIDE>(getBLOCKDATA(block));
    if(side == oppositeSide(attached_side))
        return PowerState::StronglyPowered;

    return PowerState::Powered;
}

const char *SwitchRenderer::getName(const BLOCK_WDATA)
{
    return "Redstone Switch";
}
