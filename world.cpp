#include <random>
#include <libndls.h>

#include "blockrenderer.h"
#include "world.h"
#include "chunk.h"
#include "settingstask.h"

World world;

World::World() : perlin_noise(0)
{
    generateSeed();
}

World::~World()
{
    free(seed);
    for(auto&& c : all_chunks)
        delete c.second;
}

void World::generateSeed()
{
    //First malloc and then free, as we may get the same again otherwise
    unsigned int *old_seed = seed;
    seed = static_cast<unsigned int*>(malloc(sizeof(*seed)));
    free(old_seed);
    printf("Seed: %d\n", *seed);
    perlin_noise.setSeed(*seed);
}

constexpr int getLocal(const int global)
{
     static_assert(Chunk::SIZE == 8, "Update the bit operations accordingly!");

     return global & 0b111;
}

constexpr int getChunk(const int global)
{
    return global >> 3;
}

BLOCK_WDATA World::getBlock(const int x, const int y, const int z) const
{
    int chunk_x = getChunk(x), chunk_y = getChunk(y), chunk_z = getChunk(z);

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(!c)
    {
        //Don't render world edges except for the top
        if(chunk_y == World::HEIGHT)
            return BLOCK_AIR;
        else
            return BLOCK_STONE;
    }

    return c->getLocalBlock(getLocal(x), getLocal(y), getLocal(z));
}

void World::setBlock(const int x, const int y, const int z, const BLOCK_WDATA block, bool set_dirty)
{
    int chunk_x = getChunk(x), chunk_y = getChunk(y), chunk_z = getChunk(z);
    int local_x = getLocal(x), local_y = getLocal(y), local_z = getLocal(z);

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(c)
        c->setLocalBlock(local_x, local_y, local_z, block, set_dirty);
    else
        pending_block_changes.push_back({chunk_x, chunk_y, chunk_z, local_x, local_y, local_z, block});
}

void World::changeBlock(const int x, const int y, const int z, const BLOCK_WDATA block)
{
    int chunk_x = getChunk(x), chunk_y = getChunk(y), chunk_z = getChunk(z);
    int local_x = getLocal(x), local_y = getLocal(y), local_z = getLocal(z);

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(c)
        c->changeLocalBlock(local_x, local_y, local_z, block);
    else
        pending_block_changes.push_back({chunk_x, chunk_y, chunk_z, local_x, local_y, local_z, block});
}

void World::setChunkVisible(int x, int y, int z)
{
    Chunk *c = findChunk(x, y, z);
    if(!c)
        c = generateChunk(x, y, z);

    if(c)
        visible_chunks.push_back(c);
}

void World::setPosition(int x, int y, int z)
{
    //In C and C++, integer division rounds towards zero, so with negative coords this will be one block off
    int chunk_x = getChunk((GLFix(x) / BLOCK_SIZE).floor()), chunk_y = getChunk((GLFix(y) / BLOCK_SIZE).floor()), chunk_z = getChunk((GLFix(z) / BLOCK_SIZE).floor());

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

bool World::blockAction(const int x, const int y, const int z)
{
    int chunk_x = getChunk(x), chunk_y = getChunk(y), chunk_z = getChunk(z);
    int local_x = getLocal(x), local_y = getLocal(y), local_z = getLocal(z);

    Chunk *c = findChunk(chunk_x, chunk_y, chunk_z);
    if(!c)
        return false;

    return global_block_renderer.action(c->getLocalBlock(local_x, local_y, local_z), local_x, local_y, local_z, *c);
}

bool World::intersect(AABB &other) const
{
    for(Chunk *c : visible_chunks)
        if(c->intersects(other))
            return true;

    return false;
}

bool World::intersectsRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, VECTOR3 &result, AABB::SIDE &side, GLFix &dist, bool ignore_water) const
{
    dist = GLFix::maxValue();
    VECTOR3 pos;
    for(Chunk *c : visible_chunks)
    {
        GLFix new_dist;
        AABB::SIDE new_side = AABB::NONE;

        if(c->intersectsRay(x, y, z, dx, dy, dz, new_dist, pos, new_side, ignore_water))
        {
            if(new_dist > dist)
                continue;

            result.x = pos.x + c->x*Chunk::SIZE;
            result.y = pos.y + c->y*Chunk::SIZE;
            result.z = pos.z + c->z*Chunk::SIZE;
            side = new_side;
            dist = new_dist;
        }
    }

    return dist != GLFix::maxValue();
}

const PerlinNoise &World::noiseGenerator() const
{
    return perlin_noise;
}

void World::clear()
{
    for(auto &&c : all_chunks)
        delete c.second;

    all_chunks.clear();
    visible_chunks.clear();

    loaded = false;
}

void World::setDirty()
{
    for(auto &&c : all_chunks)
        c.second->setDirty();
}

#define LOAD_FROM_FILE(var) if(fread(&(var), sizeof(var), 1, file) != 1) return false;
#define SAVE_TO_FILE(var) if(fwrite(&(var), sizeof(var), 1, file) != 1) return false;

bool World::loadFromFile(FILE *file)
{
    drawLoadingtext(1);

    clear();

    LOAD_FROM_FILE(*seed)
    perlin_noise.setSeed(*seed);

    unsigned int block_changes;
    LOAD_FROM_FILE(block_changes)
    pending_block_changes.resize(block_changes);

    if(fread(pending_block_changes.data(), sizeof(BLOCK_CHANGE), block_changes, file) != block_changes)
        return false;

    LOAD_FROM_FILE(field_of_view);

    for(;;)
    {
        int x, y, z;
        if(fread(&x, sizeof(x), 1, file) != 1)
            return feof(file);

        LOAD_FROM_FILE(y)
        LOAD_FROM_FILE(z)

        Chunk *c = new Chunk(x, y, z);
        if(!c->loadFromFile(file))
        {
            delete c;
            return false;
        }
        all_chunks.insert({std::tuple<int,int,int>(x, y, z), c});
    }
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

    for(auto&& c : all_chunks)
    {
        SAVE_TO_FILE(c.second->x)
        SAVE_TO_FILE(c.second->y)
        SAVE_TO_FILE(c.second->z)

        if(!c.second->saveToFile(file))
            return false;
    }

    return true;
}

void World::render()
{
    if(settings_task.getValue(SettingsTask::TICKS_ENABLED))
    {
        for(Chunk *c : visible_chunks)
            c->logic();
    }

    for(Chunk *c : visible_chunks)
        c->render();
}

Chunk* World::findChunk(int x, int y, int z) const
{
    auto&& c = all_chunks.find(std::tuple<int,int,int>{x, y, z});
    if(c == all_chunks.end())
        return nullptr;
    else
        return c->second;
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

    Chunk *c = new Chunk(x, y, z);
    if(c == nullptr)
        return c;

    c->generate();
    all_chunks.insert({std::tuple<int,int,int>(x, y, z), c});

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
