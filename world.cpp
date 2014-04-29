#include <random>
#include <libndls.h>

#include "world.h"
#include "chunk.h"

World::World() : perlin_noise(0)
{
    generateSeed();
}

World::~World()
{
    free(seed);
    for(Chunk *c : all_chunks)
        delete c;
}

void World::generateSeed()
{
    unsigned int *old_seed = seed;
    seed = static_cast<unsigned int*>(malloc(sizeof(*seed)));
    free(old_seed);
    printf("Seed: %d\n", *seed);
    perlin_noise.setSeed(*seed);
}

BLOCK_WDATA World::getBlock(int x, int y, int z) const
{
    int chunk_x = (GLFix(x) / Chunk::SIZE).floor(), chunk_y = (GLFix(y) / Chunk::SIZE).floor(), chunk_z = (GLFix(z) / Chunk::SIZE).floor();

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(!c)
    {
        //Don't render world edges except for the top
        if(chunk_y == World::HEIGHT)
            return BLOCK_AIR;
        else
            return BLOCK_STONE;
    }

    return c->getLocalBlock(x - chunk_x*Chunk::SIZE, y - chunk_y * Chunk::SIZE, z - chunk_z * Chunk::SIZE);
}

void World::setBlock(int x, int y, int z, BLOCK_WDATA block)
{
    int chunk_x = (GLFix(x) / Chunk::SIZE).floor(), chunk_y = (GLFix(y) / Chunk::SIZE).floor(), chunk_z = (GLFix(z) / Chunk::SIZE).floor();
    int local_x = x - chunk_x * Chunk::SIZE, local_y = y - chunk_y * Chunk::SIZE, local_z = z - chunk_z * Chunk::SIZE;

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(c)
    {
        c->setLocalBlock(local_x, local_y, local_z, block);

        if(local_x == 0)
            if(Chunk *c = findChunk(chunk_x - 1, chunk_y, chunk_z))
                c->setDirty();

        if(local_x == Chunk::SIZE - 1)
            if(Chunk *c = findChunk(chunk_x + 1, chunk_y, chunk_z))
                c->setDirty();

        if(local_y == 0)
            if(Chunk *c = findChunk(chunk_x, chunk_y - 1, chunk_z))
                c->setDirty();

        if(local_y == Chunk::SIZE - 1)
            if(Chunk *c = findChunk(chunk_x, chunk_y + 1, chunk_z))
                c->setDirty();

        if(local_z == 0)
            if(Chunk *c = findChunk(chunk_x, chunk_y, chunk_z - 1))
                c->setDirty();

        if(local_z == Chunk::SIZE - 1)
            if(Chunk *c = findChunk(chunk_x, chunk_y, chunk_z + 1))
                c->setDirty();
    }
    else
        pending_block_changes.push_back({chunk_x, chunk_y, chunk_z, local_x, local_y, local_z, block});
}

void World::setChunkVisible(int x, int y, int z)
{
    Chunk *c = findChunk(x, y, z);
    if(!c)
        c = generateChunk(x, y, z);

    visible_chunks.push_back(c);
}

void World::setPosition(int x, int y, int z)
{
    int chunk_x = (GLFix(x) / (Chunk::SIZE * BLOCK_SIZE)).floor(), chunk_y = (GLFix(y) / (Chunk::SIZE * BLOCK_SIZE)).floor(), chunk_z = (GLFix(z) / (Chunk::SIZE * BLOCK_SIZE)).floor();

    chunk_y = std::max(0, std::min(chunk_y, World::HEIGHT - 1));

    if(!loaded || (chunk_x != cen_x || chunk_y != cen_y || chunk_z != cen_z))
    {
        visible_chunks.clear();

        setChunkVisible(chunk_x, chunk_y, chunk_z);

        for(int dist = 1; dist <= field_of_view; ++dist)
        {
            for(int x = -dist; x <= dist; ++x)
                for(int y = -dist; y <= dist; ++y)
                    for(int z = -dist; z <= dist; ++z)
                    {
                        if(chunk_y + y < 0 || chunk_y + y >= World::HEIGHT || round(sqrt(x*x + y*y + z*z)) != dist)
                            continue;

                        setChunkVisible(chunk_x + x, chunk_y + y, chunk_z + z);
                    }
        }

        cen_x = chunk_x;
        cen_y = chunk_y;
        cen_z = chunk_z;

        loaded = true;
    }
}

bool World::intersect(AABB &other) const
{
    for(Chunk *c : visible_chunks)
        if(c->intersects(other))
            return true;

    return false;
}

bool World::intersectRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, Position &result, AABB::SIDE &side) const
{
    GLFix shortest_dist = GLFix::maxValue();
    Position pos;
    for(Chunk *c : visible_chunks)
    {
        GLFix new_dist;
        AABB::SIDE new_side;

        if(c->intersectsRay(x, y, z, dx, dy, dz, new_dist, pos, new_side) && new_dist < shortest_dist)
        {
            result.x = pos.x + c->x*Chunk::SIZE;
            result.y = pos.y + c->y*Chunk::SIZE;
            result.z = pos.z + c->z*Chunk::SIZE;
            side = new_side;
            shortest_dist = new_dist;
        }
    }

    return shortest_dist != GLFix::maxValue();
}

const PerlinNoise &World::noiseGenerator() const
{
    return perlin_noise;
}

void World::clear()
{
    for(Chunk *c : all_chunks)
        delete c;

    all_chunks.clear();
    visible_chunks.clear();

    loaded = false;
}

#define LOAD_FROM_FILE(var) if(fread(&(var), sizeof(var), 1, file) != 1) return false;
#define SAVE_TO_FILE(var) if(fwrite(&(var), sizeof(var), 1, file) != 1) return false;

bool World::loadFromFile(FILE *file)
{
    drawLoadingtext(1);

    LOAD_FROM_FILE(*seed)
    perlin_noise.setSeed(*seed);

    unsigned int block_changes;
    LOAD_FROM_FILE(block_changes)
    pending_block_changes.resize(block_changes);

    if(fread(pending_block_changes.data(), sizeof(BLOCK_CHANGE), block_changes, file) != block_changes)
        return false;

    LOAD_FROM_FILE(field_of_view);

    while(!feof(file))
    {
        int x, y, z;
        LOAD_FROM_FILE(x)
        LOAD_FROM_FILE(y)
        LOAD_FROM_FILE(z)

        Chunk *c = new Chunk(x, y, z, this);
        if(!c->loadFromFile(file))
        {
            delete c;
            return false;
        }
        all_chunks.push_back(c);
    }

    return true;
}

bool World::saveToFile(FILE *file) const
{
    drawLoadingtext(1);

    SAVE_TO_FILE(*seed)

    unsigned int block_changes = pending_block_changes.size();
    SAVE_TO_FILE(block_changes)

    if(fwrite(pending_block_changes.data(), sizeof(BLOCK_CHANGE), block_changes, file) != block_changes)
        return false;

    SAVE_TO_FILE(field_of_view);

    for(Chunk *c : all_chunks)
    {
        SAVE_TO_FILE(c->x)
        SAVE_TO_FILE(c->y)
        SAVE_TO_FILE(c->z)

        if(!c->saveToFile(file))
            return false;
    }

    return true;
}

void World::render()
{
    for(Chunk *c : visible_chunks)
        c->render();
}

Chunk* World::findChunk(int x, int y, int z) const
{
    for(Chunk *c : all_chunks)
        if(c->x == x && c->y == y && c->z == z)
            return c;

    return nullptr;
}

Chunk* World::generateChunk(int x, int y, int z)
{
    drawLoadingtext(2);

    if(Chunk *c = findChunk(x - 1, y, z))
        c->setDirty();
    if(Chunk *c = findChunk(x + 1, y, z))
        c->setDirty();
    if(Chunk *c = findChunk(x, y - 1, z))
        c->setDirty();
    if(Chunk *c = findChunk(x, y + 1, z))
        c->setDirty();
    if(Chunk *c = findChunk(x, y, z - 1))
        c->setDirty();
    if(Chunk *c = findChunk(x, y, z + 1))
        c->setDirty();

    Chunk *c = new Chunk(x, y, z, this);
    c->generate();
    all_chunks.push_back(c);

    for(auto it = pending_block_changes.begin(); it != pending_block_changes.end();)
    {
        BLOCK_CHANGE &block_change = *it;
        if(c->x == block_change.chunk_x && c->y == block_change.chunk_y && c->z == block_change.chunk_z)
        {
            c->setLocalBlock(block_change.local_x, block_change.local_y, block_change.local_z, block_change.block);
            it = pending_block_changes.erase(it);
        }
        else
            ++it;
    }

    return c;
}
