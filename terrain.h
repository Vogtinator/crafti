#ifndef TERRAIN_H
#define TERRAIN_H

#include <cstdint>

#include "texturetools.h"

#define BLOCK_SIZE 120

typedef uint8_t BLOCK;
typedef uint16_t BLOCK_WDATA;

#define BLOCK_AIR 0
#define BLOCK_STONE 1
#define BLOCK_DIRT 2
#define BLOCK_SAND 3
#define BLOCK_WOOD 4
#define BLOCK_LEAVES 5
#define BLOCK_PLANKS_NORMAL 6
#define BLOCK_WALL 7
#define BLOCK_COAL_ORE 8
#define BLOCK_GOLD_ORE 9
#define BLOCK_IRON_ORE 10
#define BLOCK_DIAMOND_ORE 11
#define BLOCK_REDSTONE_ORE 12
#define BLOCK_TNT 13
#define BLOCK_SPONGE 14
#define BLOCK_PLANKS_DARK 15
#define BLOCK_PLANKS_BRIGHT 16
#define BLOCK_FURNACE 17
#define BLOCK_CRAFTING_TABLE 18
#define BLOCK_BOOKSHELF 19
#define BLOCK_GRASS 20
#define BLOCK_PUMPKIN 21
#define BLOCK_NORMAL_MAX 22

//Special blocks begin here
#define BLOCK_SPECIAL_START 127
#define BLOCK_TORCH 127 //Data: Direction (BLOCK_SIDE)
#define BLOCK_FLOWER 128 //Data: Flower type
#define BLOCK_SPIDERWEB 129
#define BLOCK_CAKE 130
#define BLOCK_MUSHROOM 131 //Data: Mushroom type
#define BLOCK_SPECIAL_MAX 132

constexpr BLOCK getBLOCK(BLOCK_WDATA bd) { return bd & 0xFF; }
constexpr uint8_t getBLOCKDATA(BLOCK_WDATA bd) { return (bd >> 8) & 0xFF; }
constexpr BLOCK_WDATA getBLOCKWDATA(BLOCK b, uint8_t data) { return (data << 8) | b; }

enum BLOCK_SIDE {
    BLOCK_FRONT = 0,
    BLOCK_BACK,
    BLOCK_LEFT,
    BLOCK_RIGHT,
    BLOCK_TOP,
    BLOCK_BOTTOM,
    BLOCK_SIDE_MAX = 6
};

//There may be more than one resolution, so compute TextureAtlasEntries for both
struct TerrainAtlasEntry {
    TextureAtlasEntry current; //For blocks
    TextureAtlasEntry resized; //For GUI and other elements which shouldn't scale to the texture
};

extern const char *block_names[];
extern TerrainAtlasEntry block_textures[BLOCK_NORMAL_MAX][BLOCK_SIDE_MAX];
extern TerrainAtlasEntry terrain_atlas[16][16];

//terrain_resized is always 256x256 pixels
extern TEXTURE *terrain_current, *terrain_resized;

void terrainInit(const char *texture_path);
void terrainUninit();

#endif // TERRAIN_H
