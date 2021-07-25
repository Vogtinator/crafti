#include "torchrenderer.h"
#include "textures/part_fire.h"
#include "worldtask.h"
#include "particle.h"

constexpr GLFix TorchRenderer::torch_height;
constexpr GLFix TorchRenderer::torch_width;

void TorchRenderer::renderSpecialBlock(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z, Chunk &c)
{
    TextureAtlasEntry &tex = terrain_atlas[0][5].current;

    renderTorch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)), x, y, z, tex, c, true);
}

AABB TorchRenderer::getAABB(const BLOCK_WDATA block, GLFix x, GLFix y, GLFix z)
{
    constexpr GLFix center = BLOCK_SIZE / 2;

    switch(static_cast<BLOCK_SIDE>(getBLOCKDATA(block)))
    {
    default:
    case BLOCK_TOP:
        return {x + center - torch_width/2, y, z + center - torch_width/2, x + center + torch_width/2, y + torch_height, z + center + torch_width/2};
    case BLOCK_BOTTOM:
        return {x + center - torch_width/2, y + BLOCK_SIZE - torch_height, z + center - torch_width/2, x + center + torch_width/2, y + BLOCK_SIZE, z + center + torch_width/2};
    case BLOCK_BACK:
    case BLOCK_FRONT:
        return {x + center - torch_width/2, y, z, x + center + torch_width/2, y + torch_height, z + BLOCK_SIZE};
    case BLOCK_LEFT:
    case BLOCK_RIGHT:
        return {x, y, z + center - torch_width/2, x + BLOCK_SIZE, y + torch_height, z + center + torch_width/2};
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

void TorchRenderer::renderTorch(const BLOCK_SIDE side, const GLFix x, const GLFix y, const GLFix z, TextureAtlasEntry tex, Chunk &c, bool flame)
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
        glTranslatef(0, 0, -BLOCK_SIZE/6);
        nglRotateX(45);
        break;
    case BLOCK_FRONT:
        glTranslatef(0, 0, BLOCK_SIZE/6);
        nglRotateX(315);
        break;
    case BLOCK_LEFT:
        glTranslatef(BLOCK_SIZE/6, 0, 0);
        nglRotateZ(45);
        break;
    case BLOCK_RIGHT:
        glTranslatef(-BLOCK_SIZE/6, 0, 0);
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

    if(flame)
    {
        VECTOR3 flame_center = {BLOCK_SIZE / 2, torch_height, BLOCK_SIZE / 2};
        nglMultMatVectRes(transformation, &flame_center, &flame_center);

        Chunk::Animation animation {flame_center.x, flame_center.y, flame_center.z,
                    [] (GLFix x, GLFix y, GLFix z, Chunk &) {
            static const GLFix steps[] = {30, 15, 25, 10, 32, 27, 17, 28};
            const auto step_count = sizeof(steps) / sizeof(steps[0]);
            const int frames_per_step = 8;
            auto counter = world_task.frameCount();

            // Show each step for frames_per_step frames, interpolating linearly.
            // Mix in X/Y/Z to give each torch separate behaviour.
            auto step = counter / frames_per_step + int(x + y + z) / BLOCK_SIZE;
            GLFix t = GLFix(counter % frames_per_step) / frames_per_step;
            const GLFix flame_size = (GLFix(1) - t) * steps[step % step_count]
                                     + t * steps[(step + 1) % step_count];

            // Avoid glitches due to rounding errors
            static const TextureAtlasEntry tex = textureArea(1, 1, part_fire.width - 1, part_fire.height - 1);

            glBindTexture(&part_fire);

            // Render always facing the camera and right side up,
            // based on the coordinates of the bottom center.
            Particle::render({x, y + flame_size/2, z}, flame_size, tex);
        }};

        c.addAnimation(animation);
    }

    glPopMatrix();
}
