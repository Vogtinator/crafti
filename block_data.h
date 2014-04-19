#ifndef BLOCK_DATA_H
#define BLOCK_DATA_H

#include <cstdint>

#include "textureatlas.h"

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
#define BLOCK_NORMAL_MAX 21

//Special blocks begin here
#define BLOCK_SPECIAL_START 127
#define BLOCK_TORCH 127

constexpr BLOCK getBLOCK(BLOCK_WDATA bd) { return bd & 0xFF; }
constexpr uint8_t getBLOCKDATA(BLOCK_WDATA bd) { return bd >> 8 & 0xFF; }
constexpr BLOCK_WDATA getBLOCKWDATA(BLOCK b, uint8_t data) { return data << 8 | b; }
constexpr bool isSpecialBlock(BLOCK b) { return b >= BLOCK_SPECIAL_START; }
constexpr bool isBlockTransparent(BLOCK b) { return b == BLOCK_AIR || isSpecialBlock(b); }

enum BLOCK_SIDE {
    BLOCK_FRONT,
    BLOCK_BACK,
    BLOCK_LEFT,
    BLOCK_RIGHT,
    BLOCK_TOP,
    BLOCK_BOTTOM
};
#define BLOCK_SIDE_MAX 6

extern const char *block_names[];
extern COLOR block_colors[BLOCK_NORMAL_MAX][BLOCK_SIDE_MAX];
extern TextureAtlasEntry block_textures[BLOCK_NORMAL_MAX][BLOCK_SIDE_MAX];

void init_blockData(TEXTURE *texture);

#endif // BLOCK_DATA_H
