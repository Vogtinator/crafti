#include "terrain.h"

#include "textures/terrain.h"

const char *block_names[] =
{
    "Air",
    "Stone",
    "Dirt",
    "Sand",
    "Wood",
    "Leaves",
    "Normal Planks",
    "Wall",
    "Coal Ore",
    "Gold Ore",
    "Iron Ore",
    "Diamond Ore",
    "Redstone Ore",
    "TNT",
    "Sponge",
    "Dark Planks",
    "Bright Planks",
    "Furnace",
    "Crafting Table",
    "Bookshelf",
    "Grass",
    "Pumpkin",
    "Bedrock",
    "Glass",
    "Cobblestone",
    "Glowstone",
    "Iron block",
    "Gold block",
    "Diamond block",
    "Netherrack"
};

#define BT_FRONT 1
#define BT_BACK 2
#define BT_LEFT 4
#define BT_RIGHT 8
#define BT_TOP 16
#define BT_BOTTOM 32

struct BLOCK_TEXTURE {
    BLOCK block;
    uint8_t sides;
};

#define NON {BLOCK_AIR, 0}
#define ALL(block) {block, BT_TOP | BT_BOTTOM | BT_LEFT | BT_RIGHT | BT_FRONT | BT_BACK}
#define TOP(block) {block, BT_TOP}
#define BOT(block) {block, BT_BOTTOM}
#define LEF(block) {block, BT_LEFT}
#define RIG(block) {block, BT_RIGHT}
#define FRO(block) {block, BT_FRONT}
#define BAC(block) {block, BT_BACK}
#define TAB(block) {block, BT_TOP | BT_BOTTOM}
#define SID(block) {block, BT_FRONT | BT_BACK | BT_LEFT | BT_RIGHT}
#define SWF(block) {block, BT_BACK | BT_LEFT | BT_RIGHT}
#define AWF(block) {block, BT_TOP | BT_BOTTOM | BT_LEFT | BT_RIGHT | BT_BACK}

//Maps location in texture atlas to block ID
BLOCK_TEXTURE texture_atlas[][16] =
{
    { TOP(BLOCK_GRASS), ALL(BLOCK_STONE), ALL(BLOCK_DIRT), SID(BLOCK_GRASS), ALL(BLOCK_PLANKS_NORMAL), NON, NON, ALL(BLOCK_WALL), ALL(BLOCK_TNT), TOP(BLOCK_TNT), BOT(BLOCK_TNT), NON, NON, NON, NON, NON },
    { NON, ALL(BLOCK_BEDROCK), ALL(BLOCK_SAND), ALL(BLOCK_COBBLESTONE), SID(BLOCK_WOOD), TAB(BLOCK_WOOD), ALL(BLOCK_IRON), ALL(BLOCK_GOLD), ALL(BLOCK_DIAMOND), NON, NON, NON, NON, NON, NON, NON },
    { ALL(BLOCK_GOLD_ORE), ALL(BLOCK_IRON_ORE), ALL(BLOCK_COAL_ORE), FRO(BLOCK_BOOKSHELF), NON, NON, NON, NON, NON, NON, NON, TAB(BLOCK_CRAFTING_TABLE), FRO(BLOCK_FURNACE), SWF(BLOCK_FURNACE), NON, NON },
    { ALL(BLOCK_SPONGE), ALL(BLOCK_GLASS), ALL(BLOCK_DIAMOND_ORE), ALL(BLOCK_REDSTONE_ORE), NON, ALL(BLOCK_LEAVES), NON, NON, NON, NON, NON, SID(BLOCK_CRAFTING_TABLE), FRO(BLOCK_CRAFTING_TABLE), NON, TOP(BLOCK_FURNACE), NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, TAB(BLOCK_PUMPKIN), ALL(BLOCK_NETHERRACK), NON, ALL(BLOCK_GLOWSTONE), NON, NON, NON, NON, NON, NON},
    { NON, NON, NON, NON, NON, NON, SWF(BLOCK_PUMPKIN), FRO(BLOCK_PUMPKIN), NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, ALL(BLOCK_PLANKS_DARK), AWF(BLOCK_BOOKSHELF), NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, ALL(BLOCK_PLANKS_BRIGHT), NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON }
};

TerrainAtlasEntry block_textures[BLOCK_NORMAL_LAST + 1][BLOCK_SIDE_LAST + 1];
TerrainAtlasEntry terrain_atlas[16][16];

TEXTURE *terrain_current, *terrain_resized;

//Some textures have a different color in different biomes. We have to make them green. Grey grass just looks so unhealty
static void makeGreen(TEXTURE &texture, const int x, const int y, const int w, const int h)
{
    RGB green = { 0.5f, 0.8f, 0.3f };
    for(int x1 = x; x1 < w + x; x1++)
        for(int y1 = y; y1 < h + y; y1++)
        {
            RGB grey = rgbColor(texture.bitmap[x1 + y1*texture.width]);
            grey.r *= green.r;
            grey.g *= green.g;
            grey.b *= green.b;
            texture.bitmap[x1 + y1*texture.width] = colorRGB(grey);
        }
}

void terrainInit(const char *texture_path)
{
    terrain_current = loadTextureFromFile(texture_path);
    if(!terrain_current)
        terrain_current = &terrain; //Use default, included texture
    else
        puts("External texture loaded!");

    int fields_x = 16;
    int fields_y = 16;
    int field_width = terrain_current->width / fields_x;
    int field_height = terrain_current->height / fields_y;

    makeGreen(*terrain_current, 0, 0, field_width, field_height);
    makeGreen(*terrain_current, 5 * field_width, 3 * field_height, field_width, field_height);
    makeGreen(*terrain_current, 4 * field_width, 3 * field_height, field_width, field_height);

    if(terrain_current->width == 256 && terrain_current->height == 256)
        terrain_resized = terrain_current;
    else
        terrain_resized = resizeTexture(*terrain_current, 256, 256);

    for(int y = 0; y < fields_y; y++)
        for(int x = 0; x < fields_x; x++)
        {
            //+1 and -2 to work around GLFix inaccuracies resulting in rounding errors
            TerrainAtlasEntry tea = terrain_atlas[x][y] = {textureArea(x * field_width + 1, y * field_height + 1, field_width - 2, field_height - 2),
                                                            textureArea(x * 16, y * 16, 16, 16) };

            BLOCK_TEXTURE bt = texture_atlas[y][x];
            if(bt.sides == 0)
                continue;

            if(bt.sides & BT_BOTTOM)
                block_textures[bt.block][BLOCK_BOTTOM] = tea;
            if(bt.sides & BT_TOP)
                block_textures[bt.block][BLOCK_TOP] = tea;
            if(bt.sides & BT_LEFT)
                block_textures[bt.block][BLOCK_LEFT] = tea;
            if(bt.sides & BT_RIGHT)
                block_textures[bt.block][BLOCK_RIGHT] = tea;
            if(bt.sides & BT_FRONT)
                block_textures[bt.block][BLOCK_FRONT] = tea;
            if(bt.sides & BT_BACK)
                block_textures[bt.block][BLOCK_BACK] = tea;
        }

    //Slight hack, you can't assign a texture to multiple blocks
    block_textures[BLOCK_GRASS][BLOCK_BOTTOM] = block_textures[BLOCK_DIRT][BLOCK_BOTTOM];
}

void terrainUninit()
{
    if(terrain_current->width != 256 || terrain_current->height != 256)
        deleteTexture(terrain_resized);

    if(terrain_current != &terrain)
        deleteTexture(terrain_current);
}
