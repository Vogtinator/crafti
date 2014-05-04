#ifndef TERRAIN_H
#define TERRAIN_H

#include <cstdint>

#include "texturetools.h"

#define BLOCK_SIZE 120

typedef uint8_t BLOCK;
typedef uint16_t BLOCK_WDATA;

constexpr BLOCK BLOCK_AIR = 0;
constexpr BLOCK BLOCK_STONE = 1;
constexpr BLOCK BLOCK_DIRT = 2;
constexpr BLOCK BLOCK_SAND = 3;
constexpr BLOCK BLOCK_WOOD = 4;
constexpr BLOCK BLOCK_LEAVES = 5;
constexpr BLOCK BLOCK_PLANKS_NORMAL = 6;
constexpr BLOCK BLOCK_WALL = 7;
constexpr BLOCK BLOCK_COAL_ORE = 8;
constexpr BLOCK BLOCK_GOLD_ORE = 9;
constexpr BLOCK BLOCK_IRON_ORE = 10;
constexpr BLOCK BLOCK_DIAMOND_ORE = 11;
constexpr BLOCK BLOCK_REDSTONE_ORE = 12;
constexpr BLOCK BLOCK_TNT = 13;
constexpr BLOCK BLOCK_SPONGE = 14;
constexpr BLOCK BLOCK_PLANKS_DARK = 15;
constexpr BLOCK BLOCK_PLANKS_BRIGHT = 16;
constexpr BLOCK BLOCK_FURNACE = 17;
constexpr BLOCK BLOCK_CRAFTING_TABLE = 18;
constexpr BLOCK BLOCK_BOOKSHELF = 19;
constexpr BLOCK BLOCK_GRASS = 20;
constexpr BLOCK BLOCK_PUMPKIN = 21;
constexpr BLOCK BLOCK_BEDROCK = 22;
constexpr BLOCK BLOCK_GLASS = 23;
constexpr BLOCK BLOCK_NORMAL_LAST = BLOCK_GLASS;

//Special blocks begin here
constexpr int BLOCK_SPECIAL_START = 127;
constexpr BLOCK BLOCK_TORCH = 127; //Data: Direction (BLOCK_SIDE)
constexpr BLOCK BLOCK_FLOWER = 128; //Data: Flower type
constexpr BLOCK BLOCK_SPIDERWEB = 129;
constexpr BLOCK BLOCK_CAKE = 130;
constexpr BLOCK BLOCK_MUSHROOM = 131; //Data: Mushroom type
constexpr BLOCK BLOCK_DOOR = 132; //Data: (top: 1<<7) | BLOCK_SIDE
constexpr BLOCK BLOCK_SPECIAL_LAST = BLOCK_MUSHROOM;

constexpr BLOCK getBLOCK(BLOCK_WDATA bd) { return bd & 0xFF; }
constexpr uint8_t getBLOCKDATA(BLOCK_WDATA bd) { return (bd >> 8) & 0xFF; }
constexpr BLOCK_WDATA getBLOCKWDATA(BLOCK b, uint8_t data) { return (data << 8) | b; }

enum BLOCK_SIDE {
    BLOCK_FRONT=0,
    BLOCK_BACK,
    BLOCK_LEFT,
    BLOCK_RIGHT,
    BLOCK_TOP,
    BLOCK_BOTTOM
};
constexpr int BLOCK_SIDE_LAST = BLOCK_BOTTOM;

//There may be more than one resolution, so compute TextureAtlasEntries for both
struct TerrainAtlasEntry {
    TextureAtlasEntry current; //For blocks
    TextureAtlasEntry resized; //For GUI and other elements which shouldn't scale to the texture
};

extern const char *block_names[];
extern TerrainAtlasEntry block_textures[BLOCK_NORMAL_LAST + 1][BLOCK_SIDE_LAST + 1];
extern TerrainAtlasEntry terrain_atlas[16][16];

//terrain_resized is always 256x256 pixels
extern TEXTURE *terrain_current, *terrain_resized;

void terrainInit(const char *texture_path);
void terrainUninit();

#endif // TERRAIN_H
