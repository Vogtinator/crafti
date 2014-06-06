#include <cstdlib>
#include <algorithm>

#include "world.h"
#include "chunk.h"
#include "fastmath.h"
#include "blockrenderer.h"

#ifdef DEBUG
    #define debug(...) printf(__VA_ARGS__)
#else
    #define debug(...)
#endif

constexpr const int Chunk::SIZE;

Chunk::Chunk(int x, int y, int z, World *parent)
    : x(x), y(y), z(z), abs_x(x*SIZE*BLOCK_SIZE), abs_y(y*SIZE*BLOCK_SIZE), abs_z(z*SIZE*BLOCK_SIZE), parent(parent), aabb(abs_x, abs_y, abs_z, abs_x + SIZE*BLOCK_SIZE, abs_y + SIZE*BLOCK_SIZE, abs_z + SIZE*BLOCK_SIZE)
{}

static constexpr bool inBounds(int x, int y, int z)
{
    return x >= 0 && y >= 0 && z >= 0 && x < Chunk::SIZE && y < Chunk::SIZE && z < Chunk::SIZE;
}

int Chunk::getPosition(int x, int y, int z)
{
    if(pos_indices[x][y][z] == -1)
    {
        pos_indices[x][y][z] = positions.size();
        positions.emplace_back(Position{abs_x + x*BLOCK_SIZE, abs_y + y*BLOCK_SIZE, abs_z + z*BLOCK_SIZE});
    }

    return pos_indices[x][y][z];
}

void Chunk::addAlignedVertex(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c)
{
    vertices.emplace_back(IndexedVertex{getPosition(x, y, z), u, v, c});
}

void Chunk::addUnalignedVertex(const GLFix x, const GLFix y, const GLFix z, const GLFix u, const GLFix v, const COLOR c)
{
    vertices_unaligned.emplace_back(VERTEX{x, y, z, u, v, c});
}

void Chunk::addUnalignedVertex(const VERTEX &v)
{
    vertices_unaligned.push_back(v);
}

void Chunk::addAlignedVertexQuad(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c)
{
    vertices_quad.emplace_back(IndexedVertex{getPosition(x, y, z), u, v, c});
}

void Chunk::addAlignedVertexForceColor(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c)
{
    vertices_color.emplace_back(IndexedVertex{getPosition(x, y, z), u, v, c});
}

void Chunk::setLocalBlockSideRendered(const int x, const int y, const int z, const BLOCK_SIDE_BITFIELD side)
{
    sides_rendered[x][y][z] |= side;
}

bool Chunk::isLocalBlockSideRendered(const int x, const int y, const int z, const BLOCK_SIDE_BITFIELD side)
{
    return sides_rendered[x][y][z] & side;
}

void Chunk::buildGeometry()
{
    drawLoadingtext(5);

    std::fill(pos_indices[0][0] + 0, pos_indices[SIZE][SIZE] + SIZE + 1, -1);

    positions.clear();
    vertices.clear();
    vertices_quad.clear();
    vertices_color.clear();
    vertices_unaligned.clear();

    debug("Updating chunk %d:%d:%d...\n", x, y, z);

    debug("\tUpdating geometry...\t");

    //Bottom of world doesn't need to be drawn
    int y_start = this->y == 0 ? 0 : -1;

    //Now go through map and search for transparent blocks and draw only the sides adjacent to them
    for(int x = -1; x <= SIZE; x++)
    {
        for(int y = y_start; y <= SIZE; y++)
        {
            for(int z = -1; z <= SIZE; z++)
            {
                BLOCK_WDATA block = getGlobalBlockRelative(x, y, z), block1;

                if(block != BLOCK_AIR && global_block_renderer.isOpaque(block))
                    continue;

                if(inBounds(x - 1, y, z) && (block1 = blocks[x - 1][y][z]) != BLOCK_AIR && !(sides_rendered[x - 1][y][z] & BLOCK_RIGHT_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x - 1, y, z, BLOCK_RIGHT, *this);

                if(inBounds(x + 1, y, z) && (block1 = blocks[x + 1][y][z]) != BLOCK_AIR && !(sides_rendered[x + 1][y][z] & BLOCK_LEFT_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x + 1, y, z, BLOCK_LEFT, *this);

                if(inBounds(x, y - 1, z) && (block1 = blocks[x][y - 1][z]) != BLOCK_AIR && !(sides_rendered[x][y - 1][z] & BLOCK_TOP_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x, y - 1, z, BLOCK_TOP, *this);

                if(inBounds(x, y + 1, z) && (block1 = blocks[x][y + 1][z]) != BLOCK_AIR && !(sides_rendered[x][y + 1][z] & BLOCK_BOTTOM_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x, y + 1, z, BLOCK_BOTTOM, *this);

                if(inBounds(x, y, z - 1) && (block1 = blocks[x][y][z - 1]) != BLOCK_AIR && !(sides_rendered[x][y][z - 1] & BLOCK_BACK_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x, y, z - 1, BLOCK_BACK, *this);

                if(inBounds(x, y, z + 1) && (block1 = blocks[x][y][z + 1]) != BLOCK_AIR && !(sides_rendered[x][y][z + 1] & BLOCK_FRONT_BIT))
                    global_block_renderer.geometryNormalBlock(block1, x, y, z + 1, BLOCK_FRONT, *this);
            }
        }
    }

    //Special blocks
    GLFix pos_x = abs_x;
    for(int x = 0; x < SIZE; x++, pos_x += BLOCK_SIZE)
    {
        GLFix pos_y = abs_y;
        for(int y = 0; y < SIZE; y++, pos_y += BLOCK_SIZE)
        {
            GLFix pos_z = abs_z;
            for(int z = 0; z < SIZE; z++, pos_z += BLOCK_SIZE)
            {
                BLOCK_WDATA block = blocks[x][y][z];

                if(getBLOCK(block) == BLOCK_AIR)
                    continue;

                if(global_block_renderer.isOpaque(getGlobalBlockRelative(x - 1, y, z))
                    && global_block_renderer.isOpaque(getGlobalBlockRelative(x + 1, y, z))
                    && global_block_renderer.isOpaque(getGlobalBlockRelative(x, y - 1, z))
                    && global_block_renderer.isOpaque(getGlobalBlockRelative(x, y + 1, z))
                    && global_block_renderer.isOpaque(getGlobalBlockRelative(x, y, z - 1))
                    && global_block_renderer.isOpaque(getGlobalBlockRelative(x, y, z + 1)))
                    continue;

                global_block_renderer.renderSpecialBlock(block, pos_x, pos_y, pos_z, *this);
            }
        }
    }

    std::fill(sides_rendered[0][0] + 0, sides_rendered[SIZE - 1][SIZE - 1] + SIZE, 0);

    positions_transformed.resize(positions.size());
    positions_perspective.resize(positions.size());

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

        //TODO: Hack: This doesn't work as expected
        /*if(backface_culling && nglIsBackface(&t0, &t1, &v1))
            return false;*/

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

void Chunk::logic()
{
    tick_counter -= 1;
    if(tick_counter == 0)
    {
        tick_counter = 30; //Do a tick every 30th frame

        for(int x = 0; x < SIZE; x++)
            for(int y = 0; y < SIZE; y++)
                for(int z = 0; z < SIZE; z++)
                {
                    BLOCK_WDATA block = blocks[x][y][z];
                    if(block != BLOCK_AIR)
                        global_block_renderer.tick(block, x, y, z, *this);
                }
    }
}

void Chunk::render()
{
    if(__builtin_expect(render_dirty, 0))
        buildGeometry();

    //If there's nothing to render, skip it completely
    if(positions.size() == 0 && vertices_unaligned.size() == 0)
        return;

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

    nglForceColor(true);
    const IndexedVertex *v = vertices_color.data();
    for(unsigned int i = 0; i < vertices_color.size(); i += 4, v += 4)
    {
        if(drawTriangle(v[0], v[1], v[2], true))
            drawTriangle(v[2], v[3], v[0], false);
    }
    nglForceColor(false);

    //Same, but with textures
    v = vertices.data();
    for(unsigned int i = 0; i < vertices.size(); i += 4, v += 4)
    {
        //If it's a billboard texture, skip backface culling
        if(drawTriangle(v[0], v[1], v[2], v[0].c != 0xFFFF))
            drawTriangle(v[2], v[3], v[0], false);
    }

    //Now do the same again, but with a different texture bound
    glBindTexture(terrain_quad);

    v = vertices_quad.data();
    for(unsigned int i = 0; i < vertices_quad.size(); i += 4, v += 4)
    {
        //If it's a billboard texture, skip backface culling
        if(drawTriangle(v[0], v[1], v[2], v[0].c != 0xFFFF))
            drawTriangle(v[2], v[3], v[0], false);
    }

    glBindTexture(terrain_current);

    const VERTEX *ve = vertices_unaligned.data();
    for(unsigned int i = 0; i < vertices_unaligned.size(); i += 4, ve += 4)
    {
        nglMultMatVectRes(transformation, ve + 0, &v1);
        nglMultMatVectRes(transformation, ve + 1, &v2);
        nglMultMatVectRes(transformation, ve + 2, &v3);

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

        //If it's a billboard texture, skip backface culling
        if(nglDrawTriangle(&v1, &v2, &v3, ve[0].c != 0xFFFF))
        {
            nglMultMatVectRes(transformation, ve + 3, &v4);
            v4.u = ve[3].u;
            v4.v = ve[3].v;
            v4.c = ve[3].c;

            nglDrawTriangle(&v3, &v4, &v1, false);
        }
    }
}

BLOCK_WDATA Chunk::getLocalBlock(const int x, const int y, const int z) const
{
    return blocks[x][y][z];
}

void Chunk::setLocalBlock(const int x, const int y, const int z, const BLOCK_WDATA block, bool set_dirty)
{
    blocks[x][y][z] = block;
    if(!set_dirty)
        return;

    setDirty();

    if(x == 0)
        if(Chunk *c = parent->findChunk(this->x - 1, this->y, this->z))
            c->setDirty();

    if(x == Chunk::SIZE - 1)
        if(Chunk *c = parent->findChunk(this->x + 1, this->y, this->z))
            c->setDirty();

    if(y == 0)
        if(Chunk *c = parent->findChunk(this->x, this->y - 1, this->z))
            c->setDirty();

    if(y == Chunk::SIZE - 1)
        if(Chunk *c = parent->findChunk(this->x, this->y + 1, this->z))
            c->setDirty();

    if(z == 0)
        if(Chunk *c = parent->findChunk(this->x, this->y, this->z - 1))
            c->setDirty();

    if(z == Chunk::SIZE - 1)
        if(Chunk *c = parent->findChunk(this->x, this->y, this->z + 1))
            c->setDirty();
}

void Chunk::changeLocalBlock(const int x, const int y, const int z, const BLOCK_WDATA block)
{
    BLOCK_WDATA current_block = blocks[x][y][z];
    setLocalBlock(x, y, z, block);
    global_block_renderer.removedBlock(current_block, x, y, z, *this);
    global_block_renderer.addedBlock(block, x, y, z, *this);
}

void Chunk::changeGlobalBlockRelative(const int x, const int y, const int z, const BLOCK_WDATA block)
{
    if(inBounds(x, y, z))
        return changeLocalBlock(x, y, z, block);

    parent->changeBlock(x + this->x*SIZE, y + this->y*SIZE, z + this->z*SIZE, block);
}

BLOCK_WDATA Chunk::getGlobalBlockRelative(const int x, const int y, const int z) const
{
    if(inBounds(x, y, z))
        return getLocalBlock(x, y, z);

    return parent->getBlock(x + this->x*SIZE, y + this->y*SIZE, z + this->z*SIZE);
}

void Chunk::setGlobalBlockRelative(const int x, const int y, const int z, const BLOCK_WDATA block, bool set_dirty)
{
    if(inBounds(x, y, z))
        return setLocalBlock(x, y, z, block, set_dirty);

    return parent->setBlock(x + this->x*SIZE, y + this->y*SIZE, z + this->z*SIZE, block, set_dirty);
}

//Ignores any non-obstacle blocks
bool Chunk::intersects(AABB &other)
{
    if(!aabb.intersects(other))
        return false;

    AABB aabb;
    aabb.low_x = abs_x;

    for(unsigned int x = 0; x < SIZE; x++, aabb.low_x += BLOCK_SIZE)
    {
        aabb.high_x = aabb.low_x + BLOCK_SIZE;

        aabb.low_y = abs_y;

        for(unsigned int y = 0; y < SIZE; y++, aabb.low_y += BLOCK_SIZE)
        {
            aabb.high_y = aabb.low_y + BLOCK_SIZE;

            aabb.low_z = abs_z;

            for(unsigned int z = 0; z < SIZE; z++, aabb.low_z += BLOCK_SIZE)
            {
                aabb.high_z = aabb.low_z + BLOCK_SIZE;

                const BLOCK_WDATA block = blocks[x][y][z];

                if(!global_block_renderer.isObstacle(block))
                    continue;

                if(global_block_renderer.isBlockShaped(block))
                {
                    if(aabb.intersects(other))
                        return true;
                }
                else if(global_block_renderer.getAABB(block, aabb.low_x, aabb.low_y, aabb.low_z).intersects(other))
                    return true;
            }
        }
    }

    return false;
}

bool Chunk::intersectsRay(GLFix rx, GLFix ry, GLFix rz, GLFix dx, GLFix dy, GLFix dz, GLFix &dist, Position &pos, AABB::SIDE &side, bool ignore_water)
{
    GLFix shortest_dist;
    if(aabb.intersectsRay(rx, ry, rz, dx, dy, dz, shortest_dist) == AABB::NONE)
        return false;

    shortest_dist = GLFix::maxValue();

    AABB aabb;
    aabb.low_x = abs_x;

    for(unsigned int x = 0; x < SIZE; x++, aabb.low_x += BLOCK_SIZE)
    {
        aabb.high_x = aabb.low_x + BLOCK_SIZE;

        aabb.low_y = abs_y;

        for(unsigned int y = 0; y < SIZE; y++, aabb.low_y += BLOCK_SIZE)
        {
            aabb.high_y = aabb.low_y + BLOCK_SIZE;

            aabb.low_z = abs_z;

            for(unsigned int z = 0; z < SIZE; z++, aabb.low_z += BLOCK_SIZE)
            {
                aabb.high_z = aabb.low_z + BLOCK_SIZE;

                BLOCK_WDATA block = blocks[x][y][z];

                if(block == BLOCK_AIR || (getBLOCK(block) == BLOCK_WATER && ignore_water))
                    continue;

                AABB test = aabb;
                if(!global_block_renderer.isBlockShaped(block))
                    test = global_block_renderer.getAABB(block, aabb.low_x, aabb.low_y, aabb.low_z);

                GLFix new_dist;
                AABB::SIDE new_side = test.intersectsRay(rx, ry, rz, dx, dy, dz, new_dist);
                if(new_side != AABB::NONE)
                {
                    if(new_dist < shortest_dist)
                    {
                        pos.x.fromInt(x);
                        pos.y.fromInt(y);
                        pos.z.fromInt(z);
                        side = new_side;
                        shortest_dist = new_dist;
                    }
                }
            }
        }
    }

    if(shortest_dist == GLFix::maxValue())
        return false;

    dist = shortest_dist;
    return true;
}

void Chunk::generate()
{
    //Everything air
    std::fill(blocks[0][0] + 0, blocks[SIZE - 1][SIZE - 1] + SIZE, BLOCK_AIR);

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

            int y = 0;

            //Bottom layer of lowest chunk is bedrock
            if(this->y == 0)
                blocks[x][y++][z] = BLOCK_BEDROCK;

            for(; y < height_here; y++)
            {
                int to_surface = height_left - y;

                //Deep underground
                if(to_surface > 5)
                {
                    noise_val = noise.noise(GLFix(x)/Chunk::SIZE + this->x, GLFix(z)/Chunk::SIZE + this->z, 10.5f);
                    if(noise_val < GLFix(0.2f))
                    {
                        noise_val = noise.noise(GLFix(x)/Chunk::SIZE + this->x, GLFix(z)/Chunk::SIZE + this->z, 1000.5f);

                        uint8_t test = (noise_val.value>>4) & 0xF;

                        if(test < 1)
                            blocks[x][y][z] = BLOCK_DIAMOND_ORE;
                        else if(test < 2)
                            blocks[x][y][z] = BLOCK_REDSTONE_ORE;
                        else if(test < 5)
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

bool Chunk::isBlockPowered(const int x, const int y, const int z)
{
    return getPOWERSTATE(getGlobalBlockRelative(x - 1, y, z))
            || getPOWERSTATE(getGlobalBlockRelative(x + 1, y, z))
            || getPOWERSTATE(getGlobalBlockRelative(x, y - 1, z))
            || getPOWERSTATE(getGlobalBlockRelative(x, y + 1, z))
            || getPOWERSTATE(getGlobalBlockRelative(x, y, z - 1))
            || getPOWERSTATE(getGlobalBlockRelative(x, y, z + 1));
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
