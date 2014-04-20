#include <cstdlib>

#include "world.h"
#include "chunk.h"
#include "fastmath.h"

#define debug(...)
//#define debug(...) printf(__VA_ARGS__)

constexpr const int Chunk::SIZE;

Chunk::Chunk(int x, int y, int z, World *parent)
    : x(x), y(y), z(z), abs_x(x*SIZE*BLOCK_SIZE), abs_y(y*SIZE*BLOCK_SIZE), abs_z(z*SIZE*BLOCK_SIZE), parent(parent), aabb(abs_x, abs_y, abs_z, abs_x + SIZE*BLOCK_SIZE, abs_y + SIZE*BLOCK_SIZE, abs_z + SIZE*BLOCK_SIZE)
{}

RGB operator *(const RGB& rgb, const GLFix f)
{
    return { rgb.r * f, rgb.g * f, rgb.b * f };
}

COLOR operator *(const COLOR c, const GLFix f)
{
    RGB rgb = rgbColor(c) * f;
    return colorRGB(rgb.r, rgb.g, rgb.b);
}

#define IN_BOUNDS(x, y, z) (x >= 0 && y >= 0 && z >= 0 && x < SIZE && y < SIZE && z < SIZE)

int Chunk::getPosition(GLFix x, GLFix y, GLFix z)
{
    if(pos_indices[x][y][z] == -1)
    {
        pos_indices[x][y][z] = positions.size();
        positions.emplace_back(Position{abs_x + x*BLOCK_SIZE, abs_y + y*BLOCK_SIZE, abs_z + z*BLOCK_SIZE});
    }

    return pos_indices[x][y][z];
}

void Chunk::addAlignedVertex(const GLFix x, const GLFix y, const GLFix z, GLFix u, GLFix v, const COLOR c)
{
    vertices.emplace_back(IndexedVertex{getPosition(x, y, z), u, v, c});
}

void Chunk::geometrySpecialBlock(BLOCK_WDATA block, unsigned int x, unsigned int y, unsigned int z, BLOCK_SIDE side)
{
    if(side != BLOCK_SIDE_MAX)
        return;

    uint8_t data = getBLOCKDATA(block);
    GLFix posX = abs_x + GLFix(x) * BLOCK_SIZE;
    GLFix posY = abs_y + GLFix(y) * BLOCK_SIZE;
    GLFix posZ = abs_z + GLFix(z) * BLOCK_SIZE;

    TextureAtlasEntry tex;
    bool isBillboard = false;

    //For BLOCK_CAKE
    const GLFix cake_height = BLOCK_SIZE / 16 * 8;
    const GLFix cake_width = BLOCK_SIZE / 16 * 14;
    const GLFix cake_offset = (GLFix(BLOCK_SIZE) - cake_width) / 2;
    const TextureAtlasEntry &cake_top = terrain_atlas[9][7];
    const TextureAtlasEntry &cake_sid = terrain_atlas[10][7];
    const TextureAtlasEntry &cake_bot = terrain_atlas[12][7];

    //For BLOCK_TORCH
    std::vector<VERTEX> torch_vertices;

    switch(getBLOCK(block))
    {
    case BLOCK_FLOWER:
        isBillboard = true;
        tex = terrain_atlas[data ? 13 : 12][0];
        break;
    case BLOCK_SPIDERWEB:
        isBillboard = true;
        tex = terrain_atlas[11][0];
        break;
    case BLOCK_MUSHROOM:
        isBillboard = true;
        tex = terrain_atlas[data ? 13 : 12][1];
        break;
    case BLOCK_CAKE:
        vertices_not_aligned.push_back({posX, posY, posZ + cake_offset, cake_sid.left, cake_sid.bottom, 0xF000});
        vertices_not_aligned.push_back({posX, posY + BLOCK_SIZE, posZ + cake_offset, cake_sid.left, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + BLOCK_SIZE, posZ + cake_offset, cake_sid.right, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ + cake_offset, cake_sid.right, cake_sid.bottom, 0xF000});

        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ + cake_offset + cake_width, cake_sid.left, cake_sid.bottom, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + BLOCK_SIZE, posZ + cake_offset + cake_width, cake_sid.left, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX, posY + BLOCK_SIZE, posZ + cake_offset + cake_width, cake_sid.right, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX, posY, posZ + cake_offset + cake_width, cake_sid.right, cake_sid.bottom, 0xF000});

        vertices_not_aligned.push_back({posX + cake_offset, posY, posZ + BLOCK_SIZE, cake_sid.left, cake_sid.bottom, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset, posY + BLOCK_SIZE, posZ + BLOCK_SIZE, cake_sid.left, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset, posY + BLOCK_SIZE, posZ, cake_sid.right, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset, posY, posZ, cake_sid.right, cake_sid.bottom, 0xF000});

        vertices_not_aligned.push_back({posX + cake_offset + cake_width, posY, posZ, cake_sid.left, cake_sid.bottom, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset + cake_width, posY + BLOCK_SIZE, posZ, cake_sid.left, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset + cake_width, posY + BLOCK_SIZE, posZ + BLOCK_SIZE, cake_sid.right, cake_sid.top, 0xF000});
        vertices_not_aligned.push_back({posX + cake_offset + cake_width, posY, posZ + BLOCK_SIZE, cake_sid.right, cake_sid.bottom, 0xF000});

        vertices_not_aligned.push_back({posX, posY + cake_height, posZ, cake_top.left, cake_top.bottom, 0xF000});
        vertices_not_aligned.push_back({posX, posY + cake_height, posZ + BLOCK_SIZE, cake_top.left, cake_top.top, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + cake_height, posZ + BLOCK_SIZE, cake_top.right, cake_top.top, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + cake_height, posZ, cake_top.right, cake_top.bottom, 0xF000});

        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ, cake_bot.left, cake_bot.bottom, 0xF000});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ + BLOCK_SIZE, cake_bot.left, cake_bot.top, 0xF000});
        vertices_not_aligned.push_back({posX, posY, posZ + BLOCK_SIZE, cake_bot.right, cake_bot.top, 0xF000});
        vertices_not_aligned.push_back({posX, posY, posZ, cake_bot.right, cake_bot.bottom, 0xF000});

        break;
    case BLOCK_TORCH:
        isBillboard = false;
        tex = terrain_atlas[0][5];

        glPushMatrix();
        glLoadIdentity();

        glTranslatef(posX + BLOCK_SIZE/2, posY + BLOCK_SIZE/2, posZ + BLOCK_SIZE/2);

        torch_vertices.reserve(16);

        torch_vertices.push_back({0, 0, BLOCK_SIZE/2, tex.left, tex.bottom, 0xFFFF});
        torch_vertices.push_back({0, BLOCK_SIZE, BLOCK_SIZE/2, tex.left, tex.top, 0xFFFF});
        torch_vertices.push_back({BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE/2, tex.right, tex.top, 0xFFFF});
        torch_vertices.push_back({BLOCK_SIZE, 0, BLOCK_SIZE/2, tex.right, tex.bottom, 0xFFFF});

        torch_vertices.push_back({BLOCK_SIZE/2, 0, BLOCK_SIZE, tex.left, tex.bottom, 0xFFFF});
        torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, BLOCK_SIZE, tex.left, tex.top, 0xFFFF});
        torch_vertices.push_back({BLOCK_SIZE/2, BLOCK_SIZE, 0, tex.right, tex.top, 0xFFFF});
        torch_vertices.push_back({BLOCK_SIZE/2, 0, 0, tex.right, tex.bottom, 0xFFFF});

        switch(static_cast<BLOCK_SIDE>(data))
        {
        default:
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
            v1.u = v.u;
            v1.v = v.v;
            v1.c = v.c;
            vertices_not_aligned.push_back(v1);
        }

        glPopMatrix();

        break;
    }

    if(isBillboard)
    {
        //0xFFFF: Black = transparent and no backface culling
        vertices_not_aligned.push_back({posX, posY, posZ, tex.left, tex.bottom, 0xFFFF});
        vertices_not_aligned.push_back({posX, posY + BLOCK_SIZE, posZ, tex.left, tex.top, 0xFFFF});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + BLOCK_SIZE, posZ + BLOCK_SIZE, tex.right, tex.top, 0xFFFF});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ + BLOCK_SIZE, tex.right, tex.bottom, 0xFFFF});

        vertices_not_aligned.push_back({posX, posY, posZ + BLOCK_SIZE, tex.left, tex.bottom, 0xFFFF});
        vertices_not_aligned.push_back({posX, posY + BLOCK_SIZE, posZ + BLOCK_SIZE, tex.left, tex.top, 0xFFFF});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY + BLOCK_SIZE, posZ, tex.right, tex.top, 0xFFFF});
        vertices_not_aligned.push_back({posX + BLOCK_SIZE, posY, posZ, tex.right, tex.bottom, 0xFFFF});

        return;
    }
}

void Chunk::buildGeometry()
{
    drawLoadingtext(3);

    __builtin_memset(pos_indices, -1, sizeof(pos_indices));

    positions.clear();
    vertices.clear();
    vertices_not_aligned.clear();

    debug("Updating chunk %d:%d:%d...\n", x, y, z);

    debug("\tUpdating geometry...\t");
    int count_indices = 0;

    //Bottom of world doesn't need to be drawn
    int y_start = this->y == 0 ? 0 : -1;

    //Calculate size of buffer
    for(int x = -1; x <= SIZE; x++)
        for(int y = y_start; y <= SIZE; y++)
            for(int z = -1; z <= SIZE; z++)
            {
                BLOCK_WDATA block = getGlobalBlockRelative(x, y, z);

                //If not transparent, sides of adjacent blocks aren't visible
                if(!isBlockTransparent(block))
                    continue;

                if(IN_BOUNDS(x - 1, y, z) && (block = blocks[x - 1][y][z]) != BLOCK_AIR)
                    count_indices += 4;

                if(IN_BOUNDS(x + 1, y, z) && (block = blocks[x + 1][y][z]) != BLOCK_AIR)
                    count_indices += 4;

                if(IN_BOUNDS(x, y - 1, z) && (block = blocks[x][y - 1][z]) != BLOCK_AIR)
                    count_indices += 4;

                if(IN_BOUNDS(x, y + 1, z) && (block = blocks[x][y + 1][z]) != BLOCK_AIR)
                    count_indices += 4;

                if(IN_BOUNDS(x, y, z - 1) && (block = blocks[x][y][z - 1]) != BLOCK_AIR)
                    count_indices += 4;

                if(IN_BOUNDS(x, y, z + 1) && (block = blocks[x][y][z + 1]) != BLOCK_AIR)
                    count_indices += 4;
            }

    vertices.reserve(count_indices);

    //Now, render!
    GLFix pos_x = -1;

    for(int x = -1; x <= SIZE; x++, pos_x += 1)
    {
        GLFix pos_y = y_start;
        for(int y = y_start; y <= SIZE; y++, pos_y += 1)
        {
            GLFix pos_z = -1;
            for(int z = -1; z <= SIZE; z++, pos_z += 1)
            {
                BLOCK_WDATA block = getGlobalBlockRelative(x, y, z);

                //If not transparent, sides of adjacent blocks aren't visible
                if(!isBlockTransparent(block))
                    continue;

                #ifndef TEXTURE_SUPPORT
                    GLFix light_level = 1;

                    if((x + z) % 2 == 0)
                        light_level *= 0.8f;
                #endif

                if(IN_BOUNDS(x - 1, y, z) && (block = blocks[x - 1][y][z]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x - 1, y, z, BLOCK_RIGHT);
                    else
                    {
                        GLFix myposX = pos_x - 1;
                        GLFix myposY = pos_y;
                        GLFix myposZ = pos_z;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_RIGHT];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &ri = block_textures[block][BLOCK_RIGHT];
                        addAlignedVertex(myposX + 1, myposY, myposZ, ri.right, ri.bottom, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ, ri.right, ri.top, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ + 1, ri.left, ri.top, c);
                        addAlignedVertex(myposX + 1, myposY, myposZ + 1, ri.left, ri.bottom, c);
                    }
                }

                if(IN_BOUNDS(x + 1, y, z) && (block = blocks[x + 1][y][z]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x + 1, y, z, BLOCK_LEFT);
                    else
                    {
                        GLFix myposX = pos_x + 1;
                        GLFix myposY = pos_y;
                        GLFix myposZ = pos_z;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_LEFT];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &le = block_textures[block][BLOCK_LEFT];
                        addAlignedVertex(myposX, myposY, myposZ + 1, le.left, le.bottom, c);
                        addAlignedVertex(myposX, myposY + 1, myposZ + 1, le.left, le.top, c);
                        addAlignedVertex(myposX, myposY + 1, myposZ, le.right, le.top, c);
                        addAlignedVertex(myposX, myposY, myposZ, le.right, le.bottom, c);
                    }
                }

                if(IN_BOUNDS(x, y - 1, z) && (block = blocks[x][y - 1][z]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x, y - 1, z, BLOCK_TOP);
                    else
                    {
                        GLFix myposX = pos_x;
                        GLFix myposY = pos_y - 1;
                        GLFix myposZ = pos_z;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_TOP];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &to = block_textures[block][BLOCK_TOP];
                        addAlignedVertex(myposX, myposY + 1, myposZ, to.left, to.bottom, c);
                        addAlignedVertex(myposX, myposY + 1, myposZ + 1, to.left, to.top, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ + 1, to.right, to.top, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ, to.right, to.bottom, c);
                    }
                }

                if(IN_BOUNDS(x, y + 1, z) && (block = blocks[x][y + 1][z]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x, y + 1, z, BLOCK_BOTTOM);
                    else
                    {
                        GLFix myposX = pos_x;
                        GLFix myposY = pos_y + 1;
                        GLFix myposZ = pos_z;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_BOTTOM];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &bo = block_textures[block][BLOCK_BOTTOM];
                        addAlignedVertex(myposX + 1, myposY, myposZ, bo.left, bo.bottom, c);
                        addAlignedVertex(myposX + 1, myposY, myposZ + 1, bo.left, bo.top, c);
                        addAlignedVertex(myposX, myposY, myposZ + 1, bo.right, bo.top, c);
                        addAlignedVertex(myposX, myposY, myposZ, bo.right, bo.bottom, c);
                    }
                }

                if(IN_BOUNDS(x, y, z - 1) && (block = blocks[x][y][z - 1]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x, y, z - 1, BLOCK_BACK);
                    else
                    {
                        GLFix myposX = pos_x;
                        GLFix myposY = pos_y;
                        GLFix myposZ = pos_z - 1;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_BACK];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &ba = block_textures[block][BLOCK_BACK];
                        addAlignedVertex(myposX + 1, myposY, myposZ + 1, ba.left, ba.bottom, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ + 1, ba.left, ba.top, c);
                        addAlignedVertex(myposX, myposY + 1, myposZ + 1, ba.right, ba.top, c);
                        addAlignedVertex(myposX, myposY, myposZ + 1, ba.right, ba.bottom, c);
                    }
                }

                if(IN_BOUNDS(x, y, z + 1) && (block = blocks[x][y][z + 1]) != BLOCK_AIR)
                {
                    if(__builtin_expect(isSpecialBlock(block), 0))
                        geometrySpecialBlock(block, x, y, z + 1, BLOCK_FRONT);
                    else
                    {
                        GLFix myposX = pos_x;
                        GLFix myposY = pos_y;
                        GLFix myposZ = pos_z + 1;

                        #ifndef TEXTURE_SUPPORT
                            COLOR c = block_colors[block][BLOCK_FRONT];
                            c = c * light_level;
                        #else
                            COLOR c = 0;
                        #endif

                        TextureAtlasEntry &fr = block_textures[block][BLOCK_FRONT];
                        addAlignedVertex(myposX, myposY, myposZ, fr.left, fr.bottom, c);
                        addAlignedVertex(myposX, myposY + 1, myposZ, fr.left, fr.top, c);
                        addAlignedVertex(myposX + 1, myposY + 1, myposZ, fr.right, fr.top, c);
                        addAlignedVertex(myposX + 1, myposY, myposZ, fr.right, fr.bottom, c);
                    }
                }
            }
        }

        positions_transformed.resize(positions.size());
        positions_perspective.resize(positions.size());
    }

    //Special blocks
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            for(int z = 0; z < SIZE; z++)
            {
                BLOCK_WDATA block = blocks[x][y][z];

                if(isSpecialBlock(block))
                    geometrySpecialBlock(block, x, y, z, BLOCK_SIDE_MAX);
            }


    render_dirty = false;

    debug("Done!\n");
}

#define MAKE_VERTEX(pos, iver) { (pos).x, (pos).y, (pos).z, iver.u, iver.v, iver.c }

VERTEX Chunk::perspective(const IndexedVertex &v, Position &transformed)
{
    std::pair<Position, bool> &p = positions_perspective[v.pos];
    if(!p.second)
    {
        VERTEX ver = MAKE_VERTEX(transformed, v);
        nglPerspective(&ver);
        p.first = { ver.x, ver.y, ver.z };
        p.second = true;

        return ver;
    }

    return MAKE_VERTEX(p.first, v);
}

bool Chunk::drawTriangle(const IndexedVertex &low, const IndexedVertex &middle, const IndexedVertex &high, bool backface_culling)
{
    Position pos_low = positions_transformed[low.pos], pos_middle = positions_transformed[middle.pos], pos_high = positions_transformed[high.pos];

#ifndef Z_CLIPPING
    if(pos_low.z < GLFix(CLIP_PLANE) || pos_middle.z < GLFix(CLIP_PLANE) || pos_high.z < GLFix(CLIP_PLANE))
        return true;

    VERTEX low_p = perspective(low, pos_low), middle_p = perspective(middle, pos_middle), high_p = perspective(high, pos_high);

    if(backface_culling && nglIsBackface(&low_p, &middle_p, &high_p))
        return false;

    nglDrawTriangleZClipped(&low_p, &middle_p, &high_p);

    return true;
#else

    VERTEX invisible[3];
    IndexedVertex visible[3];
    Position *pos_visible[3];
    int count_invisible = -1, count_visible = -1;

    if(pos_low.z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = MAKE_VERTEX(pos_low, low);
    else
    {
        visible[++count_visible] = low;
        pos_visible[count_visible] = &pos_low;
    }

    if(pos_middle.z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = MAKE_VERTEX(pos_middle, middle);
    else
    {
        visible[++count_visible] = middle;
        pos_visible[count_visible] = &pos_middle;
    }

    if(pos_high.z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = MAKE_VERTEX(pos_high, high);
    else
    {
        visible[++count_visible] = high;
        pos_visible[count_visible] = &pos_high;
    }

    //Interpolated vertices
    VERTEX v1, v2;

    //Temporary vertices
    VERTEX t0, t1;

    switch(count_visible)
    {
    case -1:
        return true;

    case 0:
        t0 = MAKE_VERTEX(*pos_visible[0], visible[0]);

        nglInterpolateVertexZ(&invisible[0], &t0, &v1);
        nglInterpolateVertexZ(&invisible[1], &t0, &v2);

        t0 = perspective(visible[0], *pos_visible[0]);
        nglPerspective(&v1);
        nglPerspective(&v2);

        if(backface_culling && nglIsBackface(&t0, &v1, &v2))
            return false;

        nglDrawTriangleZClipped(&t0, &v1, &v2);
        return true;

    case 1:
        t0 = MAKE_VERTEX(*pos_visible[0], visible[0]);
        t1 = MAKE_VERTEX(*pos_visible[1], visible[1]);

        nglInterpolateVertexZ(&t0, &invisible[0], &v1);
        nglInterpolateVertexZ(&t1, &invisible[0], &v2);

        t0 = perspective(visible[0], *pos_visible[0]);
        t1 = perspective(visible[1], *pos_visible[1]);
        nglPerspective(&v1);

        if(backface_culling && nglIsBackface(&t0, &t1, &v1))
            return false;

        nglPerspective(&v2);
        nglDrawTriangleZClipped(&t0, &t1, &v1);
        nglDrawTriangleZClipped(&t1, &v1, &v2);
        return true;

    case 2:
        invisible[0] = perspective(low, pos_low);
        invisible[1] = perspective(middle, pos_middle);
        invisible[2] = perspective(high, pos_high);

        if(backface_culling && nglIsBackface(&invisible[0], &invisible[1], &invisible[2]))
            return false;

        nglDrawTriangleZClipped(&invisible[0], &invisible[1], &invisible[2]);
        return true;

    default:
        return true;
    }
#endif
}

static bool behindClip(const VERTEX &v1)
{
    return transformation->data[2][0]*v1.x + transformation->data[2][1]*v1.y + transformation->data[2][2]*v1.z + transformation->data[2][3] <= GLFix(CLIP_PLANE);
}

void Chunk::render()
{
    if(__builtin_expect(render_dirty, 0))
        buildGeometry();

    //Basic culling
    VERTEX v1{abs_x,                            abs_y,                            abs_z, 0, 0, 0},
            v2{abs_x + Chunk::SIZE * BLOCK_SIZE, abs_y,                            abs_z, 0, 0, 0},
            v3{abs_x,                            abs_y + Chunk::SIZE * BLOCK_SIZE, abs_z, 0, 0, 0},
            v4{abs_x + Chunk::SIZE * BLOCK_SIZE, abs_y + Chunk::SIZE * BLOCK_SIZE, abs_z, 0, 0, 0},
            v5{abs_x,                            abs_y,                            abs_z + Chunk::SIZE * BLOCK_SIZE, 0, 0, 0},
            v6{abs_x + Chunk::SIZE * BLOCK_SIZE, abs_y,                            abs_z + Chunk::SIZE * BLOCK_SIZE, 0, 0, 0},
            v7{abs_x,                            abs_y + Chunk::SIZE * BLOCK_SIZE, abs_z + Chunk::SIZE * BLOCK_SIZE, 0, 0, 0},
            v8{abs_x + Chunk::SIZE * BLOCK_SIZE, abs_y + Chunk::SIZE * BLOCK_SIZE, abs_z + Chunk::SIZE * BLOCK_SIZE, 0, 0, 0};

    //Z-Clipping (now, it's a bit cheaper than a full MultMatVectRes)
    if(behindClip(v1) && behindClip(v2) && behindClip(v3) && behindClip(v4) && behindClip(v5) && behindClip(v6) && behindClip(v7) && behindClip(v8))
        return;

    VERTEX v9, v10, v11, v12, v13, v14, v15, v16;

    nglMultMatVectRes(transformation, &v1, &v9);
    nglMultMatVectRes(transformation, &v2, &v10);
    nglMultMatVectRes(transformation, &v3, &v11);
    nglMultMatVectRes(transformation, &v4, &v12);
    nglMultMatVectRes(transformation, &v5, &v13);
    nglMultMatVectRes(transformation, &v6, &v14);
    nglMultMatVectRes(transformation, &v7, &v15);
    nglMultMatVectRes(transformation, &v8, &v16);

    nglPerspective(&v9);
    nglPerspective(&v10);
    nglPerspective(&v11);
    nglPerspective(&v12);
    nglPerspective(&v13);
    nglPerspective(&v14);
    nglPerspective(&v15);
    nglPerspective(&v16);

    //X and Y-Clipping

    if(v9.x < GLFix(0) && v10.x < GLFix(0) && v11.x < GLFix(0) && v12.x < GLFix(0) && v13.x < GLFix(0) && v14.x < GLFix(0) && v15.x < GLFix(0) && v16.x < GLFix(0))
        return;

    if(v9.y < GLFix(0) && v10.y < GLFix(0) && v11.y < GLFix(0) && v12.y < GLFix(0) && v13.y < GLFix(0) && v14.y < GLFix(0) && v15.y < GLFix(0) && v16.y < GLFix(0))
        return;

    if(v9.x >= SCREEN_WIDTH && v10.x >= SCREEN_WIDTH && v11.x >= SCREEN_WIDTH && v12.x >= SCREEN_WIDTH
            && v13.x >= SCREEN_WIDTH && v14.x >= SCREEN_WIDTH && v15.x >= SCREEN_WIDTH && v16.x >= SCREEN_WIDTH)
        return;

    if(v9.y >= SCREEN_HEIGHT && v10.y >= SCREEN_HEIGHT && v11.y >= SCREEN_HEIGHT && v12.y >= SCREEN_HEIGHT
            && v13.y >= SCREEN_HEIGHT && v14.y >= SCREEN_HEIGHT && v15.y >= SCREEN_HEIGHT && v16.y >= SCREEN_HEIGHT)
        return;

    std::fill(positions_perspective.begin(), positions_perspective.end(), std::make_pair<Position, bool>({0, 0, 0}, false));

    for(unsigned int i = 0; i < positions.size(); i++)
    {
        Position &position = positions[i];
        VERTEX t1{position.x, position.y, position.z, 0, 0, 0}, t2;
        nglMultMatVectRes(transformation, &t1, &t2);
        positions_transformed[i] = {t2.x, t2.y, t2.z};
    }

    const IndexedVertex *v = vertices.data();
    for(unsigned int i = 0; i < vertices.size(); i += 4, v += 4)
    {
        if(drawTriangle(v[0], v[1], v[2]))
            drawTriangle(v[2], v[3], v[0], false);
    }

    //Do these last, they may be transparent
    const VERTEX *ve = vertices_not_aligned.data();
    for(unsigned int i = 0; i < vertices_not_aligned.size(); i += 4, ve += 4)
    {
        VERTEX v1, v2, v3, v4;
        nglMultMatVectRes(transformation, ve + 0, &v1);
        nglMultMatVectRes(transformation, ve + 1, &v2);
        nglMultMatVectRes(transformation, ve + 2, &v3);
        nglMultMatVectRes(transformation, ve + 3, &v4);

        //nglMultMatVectRes doesn't copy u,v and c
        v1.u = ve[0].u;
        v1.v = ve[0].v;
        v1.c = ve[0].c;
        v2.u = ve[1].u;
        v2.v = ve[1].v;
        v2.c = ve[1].c;
        v3.u = ve[2].u;
        v3.v = ve[2].v;
        v3.c = ve[2].c;
        v4.u = ve[3].u;
        v4.v = ve[3].v;
        v4.c = ve[3].c;

        //If it's a billboard texture, skip backface culling
        if(nglDrawTriangle(&v1, &v2, &v3, ve[0].c != 0xFFFF))
            nglDrawTriangle(&v3, &v4, &v1, false);
    }
}

BLOCK_WDATA Chunk::getLocalBlock(const int x, const int y, const int z)
{
    return blocks[x][y][z];
}

void Chunk::setLocalBlock(const int x, const int y, const int z, const BLOCK_WDATA block)
{
    blocks[x][y][z] = block;
    setDirty();
}

BLOCK_WDATA Chunk::getGlobalBlockRelative(int x, int y, int z)
{
    if(IN_BOUNDS(x, y, z))
        return getLocalBlock(x, y, z);

    return parent->getBlock(x + this->x*SIZE, y + this->y*SIZE, z + this->z*SIZE);
}

void Chunk::setGlobalBlockRelative(int x, int y, int z, BLOCK_WDATA block)
{
    if(IN_BOUNDS(x, y, z))
        return setLocalBlock(x, y, z, block);

    return parent->setBlock(x + this->x*SIZE, y + this->y*SIZE, z + this->z*SIZE, block);
}

bool Chunk::intersects(AABB &other)
{
    if(!aabb.intersects(other))
        return false;

    AABB aabb;
    aabb.low_x = this->x*SIZE*BLOCK_SIZE;

    for(unsigned int x = 0; x < SIZE; x++)
    {
        aabb.high_x = aabb.low_x + BLOCK_SIZE;

        aabb.low_y = this->y*SIZE*BLOCK_SIZE;

        for(unsigned int y = 0; y < SIZE; y++)
        {
            aabb.high_y = aabb.low_y + BLOCK_SIZE;

            aabb.low_z = this->z*SIZE*BLOCK_SIZE;

            for(unsigned int z = 0; z < SIZE; z++, aabb.low_z += BLOCK_SIZE)
            {
                aabb.high_z = aabb.low_z + BLOCK_SIZE;

                if(!isBlockObstacle(blocks[x][y][z]))
                    continue;

                if(aabb.intersects(other))
                    return true;
            }

            aabb.low_y += BLOCK_SIZE;
        }

        aabb.low_x += BLOCK_SIZE;
    }

    return false;
}

bool Chunk::intersectsRay(GLFix rx, GLFix ry, GLFix rz, GLFix dx, GLFix dy, GLFix dz, GLFix &dist, Position &pos, AABB::SIDE &side)
{
    GLFix shortest_dist;
    if(!aabb.intersectsRay(rx, ry, rz, dx, dy, dz, shortest_dist))
        return false;

    shortest_dist = GLFix::maxValue();

    AABB aabb;
    aabb.low_x = this->x*SIZE*BLOCK_SIZE;

    for(unsigned int x = 0; x < SIZE; x++)
    {
        aabb.high_x = aabb.low_x + BLOCK_SIZE;

        aabb.low_y = this->y*SIZE*BLOCK_SIZE;

        for(unsigned int y = 0; y < SIZE; y++)
        {
            aabb.high_y = aabb.low_y + BLOCK_SIZE;

            aabb.low_z = this->z*SIZE*BLOCK_SIZE;

            for(unsigned int z = 0; z < SIZE; z++, aabb.low_z += BLOCK_SIZE)
            {
                aabb.high_z = aabb.low_z + BLOCK_SIZE;

                if(blocks[x][y][z] == BLOCK_AIR)
                    continue;

                GLFix new_dist;
                AABB::SIDE test = aabb.intersectsRay(rx, ry, rz, dx, dy, dz, new_dist);
                if(test != AABB::NONE)
                {
                    if(new_dist < shortest_dist)
                    {
                        pos.x.fromInt(x);
                        pos.y.fromInt(y);
                        pos.z.fromInt(z);
                        side = test;
                        shortest_dist = new_dist;
                    }
                }
            }

            aabb.low_y += BLOCK_SIZE;
        }

        aabb.low_x += BLOCK_SIZE;
    }

    if(shortest_dist == GLFix::maxValue())
        return false;

    dist = shortest_dist;
    return side;
}

void Chunk::generate()
{
    //Everything air
    memset(blocks, BLOCK_AIR, sizeof(blocks));

    debug("Generating chunk %d:%d:%d...\t", x, y, z);

    const PerlinNoise &noise = parent->noiseGenerator();

    constexpr int max_trees = (Chunk::SIZE * Chunk::SIZE) / 45;
    int trees = 0;

    for(int x = 0; x < SIZE; x++)
        for(int z = 0; z < SIZE; z++)
        {
            GLFix noise_val = noise.noise((GLFix(x)/Chunk::SIZE + this->x)/4, (GLFix(z)/Chunk::SIZE + this->z)/4, 10);
            int world_gen_min = 8, world_gen_max = World::HEIGHT * Chunk::SIZE * 0.7;
            int height = world_gen_min + (noise_val * (world_gen_max - world_gen_min)).round();
            int height_left = height - this->y * Chunk::SIZE;
            int height_here = std::min(height_left, Chunk::SIZE);

            for(int y = 0; y < height_here; y++)
            {
                int to_surface = height_left - y;

                //Deep underground
                if(to_surface > 5)
                {
                    noise_val = noise.noise(GLFix(x)/Chunk::SIZE + this->x, GLFix(z)/Chunk::SIZE + this->z, 10.5f);
                    if(noise_val < GLFix(0.4f))
                    {
                        noise_val = noise.noise(GLFix(x)/Chunk::SIZE + this->x, GLFix(z)/Chunk::SIZE + this->z, 1000.5f);

                        uint8_t test = noise_val.value & 0xF;

                        if(test < 2)
                            blocks[x][y][z] = BLOCK_DIAMOND_ORE;
                        else if(test < 5)
                            blocks[x][y][z] = BLOCK_REDSTONE_ORE;
                        else if(test < 10)
                            blocks[x][y][z] = BLOCK_IRON_ORE;
                        else
                            blocks[x][y][z] = BLOCK_COAL_ORE;
                    }
                    else
                        blocks[x][y][z] = BLOCK_STONE;
                }
                else if(height > 10)
                {
                    if(to_surface == 1)
                    {
                        blocks[x][y][z] = BLOCK_GRASS;
                        if((rand() & 0xFF) == 0x0)
                            setGlobalBlockRelative(x, y + 1, z, getBLOCKWDATA(BLOCK_FLOWER, rand() & 0x1));
                    }
                    else
                        blocks[x][y][z] = BLOCK_DIRT;
                }
                else
                    blocks[x][y][z] = BLOCK_SAND;
            }
            if(trees < max_trees && height_left >= 0 && height_left <= Chunk::SIZE && height > 5 && noise.noise(GLFix(x)/Chunk::SIZE + this->x, GLFix(z)/Chunk::SIZE + this->z, 25) < GLFix(0.3f))
            {
                makeTree(x, height_here, z);
                trees++;
            }
        }

    debug("Done!\n");
}

bool Chunk::saveToFile(FILE *file)
{
    if(fwrite(blocks, sizeof(***blocks), SIZE*SIZE*SIZE, file) == SIZE*SIZE*SIZE)
    {
        debug("Saved chunk %d:%d:%d successfully.\n", x, y, z);
        return true;
    }
    else
    {
        debug("Saving chunk %d:%d:%d failed!\n", x, y, z);
        return false;
    }
}

bool Chunk::loadFromFile(FILE *file)
{
    if(fread(blocks, sizeof(***blocks), SIZE*SIZE*SIZE, file) == SIZE*SIZE*SIZE)
    {
        debug("Loaded chunk %d:%d:%d successfully.\n", x, y, z);
        return true;
    }
    else
    {
        debug("Loading chunk %d:%d:%d failed!\n", x, y, z);
        return false;
    }
}

void Chunk::makeTree(unsigned int x, unsigned int y, unsigned int z)
{
    int max_height = World::HEIGHT * Chunk::SIZE - (y + this->y * Chunk::SIZE);

    max_height = std::min(max_height - 4, rand() & 0x5);

    if(max_height < 5)
        return;

    for(unsigned int y1 = y; y1 < y + max_height; ++y1)
        setGlobalBlockRelative(x, y1, z, BLOCK_WOOD);

    for(unsigned int y1 = y + max_height; y1 < y + max_height + 2; ++y1)
    {
        setGlobalBlockRelative(x, y1, z-1, BLOCK_LEAVES);
        setGlobalBlockRelative(x-1, y1, z-1, BLOCK_LEAVES);
        setGlobalBlockRelative(x+1, y1, z-1, BLOCK_LEAVES);
        setGlobalBlockRelative(x, y1, z, BLOCK_LEAVES);
        setGlobalBlockRelative(x-1, y1, z, BLOCK_LEAVES);
        setGlobalBlockRelative(x+1, y1, z, BLOCK_LEAVES);
        setGlobalBlockRelative(x, y1, z+1, BLOCK_LEAVES);
        setGlobalBlockRelative(x-1, y1, z+1, BLOCK_LEAVES);
        setGlobalBlockRelative(x+1, y1, z+1, BLOCK_LEAVES);
    }

    setGlobalBlockRelative(x, y + max_height + 2, z, BLOCK_LEAVES);
}
