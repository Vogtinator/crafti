#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <list>

#include "block_data.h"
#include "chunk.h"
#include "perlinnoise.h"

struct BLOCK_CHANGE {
    int chunk_x, chunk_y, chunk_z, local_x, local_y, local_z;
    BLOCK block;
};

class World
{
public:
    World();
    ~World();
    void generateSeed();
    BLOCK getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BLOCK block);
    void setPosition(int x, int y, int z);
    bool intersect(AABB &other) const;
    bool intersectRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, Position &result, AABB::SIDE &side) const;
    const PerlinNoise &noiseGenerator() const;
    void clear();
    bool loadFromFile(FILE *file);
    bool saveToFile(FILE *file) const;
    void render();
    int fieldOfView() const { return field_of_view; }
    void setFieldOfView(int fov) { field_of_view = fov; loaded = false; }

    static constexpr int HEIGHT = 5;

private:
    Chunk *findChunk(int x, int y, int z) const;
    Chunk *generateChunk(int x, int y, int z);
    void setChunkVisible(int x, int y, int z);

    bool loaded = false;
    int cen_x = 0, cen_y = 0, cen_z = 0;
    int field_of_view = 3;
    PerlinNoise perlin_noise;
    unsigned int *seed;

    std::vector<Chunk*> all_chunks;
    std::vector<Chunk*> visible_chunks;

    //If a block in a not yet loaded Chunk has been set, it's stored here until the Chunk has been loaded
    std::vector<BLOCK_CHANGE> pending_block_changes;
};

#endif // WORLD_H
