#include "block_data.h"

#include "aabb.h"

const char *block_names[BLOCK_NORMAL_MAX] =
{
    "Air",
    "Stone",
    "Sand",
    "Wood",
    "Leaves",
    "Grass",
    "Normal Planks",
    "Wall",
    "Coal Ore",
    "Gold Ore",
    "Diamod Ore",
    "Redstone Ore",
    "TNT",
    "Sponge",
    "Dark Planks",
    "Odd Planks",
    "Bright Planks"
    "Furnace",
    "Crafting Table",
    "Bookshelf",
    "Grass"
};

COLOR block_colors[BLOCK_NORMAL_MAX][BLOCK_SIDE_MAX];

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
    { NON, NON, ALL(BLOCK_SAND), BOT(BLOCK_GRASS), ALL(BLOCK_WOOD), TAB(BLOCK_WOOD), NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { ALL(BLOCK_GOLD_ORE), ALL(BLOCK_IRON_ORE), ALL(BLOCK_COAL_ORE), FRO(BLOCK_BOOKSHELF), NON, NON, NON, NON, NON, NON, NON, TAB(BLOCK_CRAFTING_TABLE), FRO(BLOCK_FURNACE), SWF(BLOCK_FURNACE), NON, NON },
    { ALL(BLOCK_SPONGE), NON, ALL(BLOCK_DIAMOND_ORE), ALL(BLOCK_REDSTONE_ORE), NON, ALL(BLOCK_LEAVES), NON, NON, NON, NON, NON, SID(BLOCK_CRAFTING_TABLE), FRO(BLOCK_CRAFTING_TABLE), NON, TOP(BLOCK_FURNACE), NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, ALL(BLOCK_PLANKS_DARK), AWF(BLOCK_BOOKSHELF), NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, ALL(BLOCK_PLANKS_BRIGHT), NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON },
    { NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON, NON }
};

TextureAtlasEntry block_textures[BLOCK_NORMAL_MAX][BLOCK_SIDE_MAX];

//Some textures have a different color in different biomes. We have to make them green. Grey grass just looks so unhealty
static void make_green(TEXTURE *texture, int x, int y, int w, int h)
{
    RGB green = { 0.5f, 0.8f, 0.3f };
    for(int x1 = x; x1 < w + x; x1++)
        for(int y1 = y; y1 < h + y; y1++)
        {
            RGB grey = rgbColor(texture->bitmap[x1 + y1*texture->width]);
            grey.r *= green.r;
            grey.g *= green.g;
            grey.b *= green.b;
            texture->bitmap[x1 + y1*texture->width] = colorRGB(grey);
        }
}

void init_blockData(TEXTURE *texture)
{
    int fields_x = 16;
    int fields_y = 16;
    int field_width = texture->width / fields_x;
    int field_height = texture->height / fields_y;

    make_green(texture, 0, 0, field_width, field_height);
    make_green(texture, 5 * field_width, 3 * field_height, field_width, field_height);

    int pixels = field_width * field_height;
    for(int y = 0; y < fields_y; y++)
        for(int x = 0; x < fields_x; x++)
        {
            BLOCK_TEXTURE bt = texture_atlas[y][x];
            if(bt.sides == 0)
                continue;

            TextureAtlasEntry tea = textureArea(x * field_width, y * field_height, field_width, field_height);

            //Get an average color of the block
            RGB sum;
            for(int tex_x = x * field_width; tex_x < x * field_width + field_width; tex_x++)
                for(int tex_y = y * field_width; tex_y < y * field_width + field_width; tex_y++)
                {
                    RGB rgb = rgbColor(texture->bitmap[tex_x + tex_y*texture->width]);
                    sum.r += rgb.r;
                    sum.g += rgb.g;
                    sum.b += rgb.b;
                }

            sum.r /= pixels;
            sum.g /= pixels;
            sum.b /= pixels;

            COLOR color = colorRGB(sum);

            if(bt.sides & BT_BOTTOM)
            {
                block_textures[bt.block][BLOCK_BOTTOM] = tea;
                block_colors[bt.block][BLOCK_BOTTOM] = color;
            }
            if(bt.sides & BT_TOP)
            {
                block_textures[bt.block][BLOCK_TOP] = tea;
                block_colors[bt.block][BLOCK_TOP] = color;
            }
            if(bt.sides & BT_LEFT)
            {
                block_textures[bt.block][BLOCK_LEFT] = tea;
                block_colors[bt.block][BLOCK_LEFT] = color;
            }
            if(bt.sides & BT_RIGHT)
            {
                block_textures[bt.block][BLOCK_RIGHT] = tea;
                block_colors[bt.block][BLOCK_RIGHT] = color;
            }
            if(bt.sides & BT_FRONT)
            {
                block_textures[bt.block][BLOCK_FRONT] = tea;
                block_colors[bt.block][BLOCK_FRONT] = color;
            }
            if(bt.sides & BT_BACK)
            {
                block_textures[bt.block][BLOCK_BACK] = tea;
                block_colors[bt.block][BLOCK_BACK] = color;
            }

        }
}
