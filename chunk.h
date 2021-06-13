#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <vector>
#include <tuple>

#include "gl.h"
#include "gldrawarray.h"
#include "terrain.h"
#include "aabb.h"

class World;

class Chunk
{
public:
    Chunk(int x, int y, int z);
    void logic();
    void render();
    void setDirty(bool dirty = true) { render_dirty = dirty; }
    bool isDirty() { return render_dirty; }
    BLOCK_WDATA getLocalBlock(const int x, const int y, const int z) const;
    void setLocalBlock(const int x, const int y, const int z, const BLOCK_WDATA block, bool set_dirty = true);
    void changeLocalBlock(const int x, const int y, const int z, const BLOCK_WDATA block); //Calls removeBlock and addBlock
    void changeGlobalBlockRelative(const int x, const int y, const int z, const BLOCK_WDATA block);
    BLOCK_WDATA getGlobalBlockRelative(const int x, const int y, const int z) const;
    void setGlobalBlockRelative(const int x, const int y, const int z, const BLOCK_WDATA block, bool set_dirty = true);
    AABB &getAABB() { return aabb; }
    bool intersects(AABB &other);
    bool intersectsRay(GLFix x, GLFix y, GLFix z, GLFix dx, GLFix dy, GLFix dz, GLFix &dist, VECTOR3 &pos, AABB::SIDE &side, bool ignore_water);
    void generate();
    bool saveToFile(FILE *file);
    bool loadFromFile(FILE *file);

    //Redstone power: See wirerenderer.cpp for details
    //Whether the block receives power from the specified side.
    bool gettingPowerFrom(const int x, const int y, const int z, BLOCK_SIDE side, bool ignore_redstone_wire = false);
    //Whether the block receives power directly or indirectly.
    bool isBlockPowered(const int x, const int y, const int z, bool ignore_redstone_wire = false);

    GLFix absX() { return abs_x; }
    GLFix absY() { return abs_y; }
    GLFix absZ() { return abs_z; }

    //Used by BlockRenderers, had to make it public because friendship is not inheritable
    void addAlignedVertex(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c);

    //Same as addAlignedVertex, but terrain_quad is the bound texture
    void addAlignedVertexQuad(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c);

    //Same as addAlignedVertex, but with nglForceColor on
    void addAlignedVertexForceColor(const int x, const int y, const int z, GLFix u, GLFix v, const COLOR c);

    // For unaligned vertices: If set, the two triangles in the quad do backface culling independently.
    // This is necessary when they're not on the same plane.
    static constexpr COLOR INDEPENDENT_TRIS = 0x0001;

    //Doesn't have to be aligned, terrain_current is bound
    void addUnalignedVertex(const GLFix x, const GLFix y, const GLFix z, const GLFix u, const GLFix v, const COLOR c);
    void addUnalignedVertex(const VERTEX &v);

    //Don't render something twice
    void setLocalBlockSideRendered(const int x, const int y, const int z, const BLOCK_SIDE_BITFIELD side);
    bool isLocalBlockSideRendered(const int x, const int y, const int z, const BLOCK_SIDE_BITFIELD side);

    static constexpr int SIZE = 8;

    const int x, y, z;

private:
    //Terrain generation
    void makeTree(unsigned int x, unsigned int y, unsigned int z);

    //Data
    unsigned int getPosition(unsigned int x, unsigned int y, unsigned int z);

    //Rendering
    void geometrySpecialBlock(BLOCK_WDATA block, unsigned int x, unsigned int y, unsigned int z, BLOCK_SIDE side);
    void buildGeometry();

    //Data
    const GLFix abs_x, abs_y, abs_z;
    AABB aabb;
    BLOCK_WDATA blocks[SIZE][SIZE][SIZE];

    //Rendering
    bool render_dirty = true;
    static int pos_indices[SIZE + 1][SIZE + 1][SIZE + 1];
    BLOCK_SIDE_BITFIELD sides_rendered[SIZE][SIZE][SIZE] = {}; //It could be that other chunks already rendered parts of our blocks
    std::vector<VECTOR3> positions;
    std::vector<ProcessedPosition> positions_processed;
    std::vector<IndexedVertex> vertices, vertices_quad, vertices_color;
    std::vector<VERTEX> vertices_unaligned; //The optimized drawing with indices doesn't work with unaligned positions
    int tick_counter = 1; //1 to trigger a tick the next frame
};

//Doesn't really belong here, but still more than everywhere else
void drawLoadingtext(const int i);

#endif // CHUNK_H
