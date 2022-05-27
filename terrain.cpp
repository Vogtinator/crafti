#include "terrain.h"

#include <libndls.h>

#include "textures/terrain.h"
#include "textures/inv_selection.h"

const char *block_names[BLOCK_NORMAL_LAST + 1] =
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
    "Netherrack",
    "Cactus"
};

struct BLOCK_TEXTURE {
    BLOCK block;
    BLOCK_SIDE_BITFIELD sides;
};

#define NON {BLOCK_AIR, 0}
#define ALL(block) {block, BLOCK_TOP_BIT | BLOCK_BOTTOM_BIT | BLOCK_LEFT_BIT | BLOCK_RIGHT_BIT | BLOCK_FRONT_BIT | BLOCK_BACK_BIT}
#define TOP(block) {block, BLOCK_TOP_BIT}
#define BOT(block) {block, BLOCK_BOTTOM_BIT}
#define LEF(block) {block, BLOCK_LEFT_BIT}
#define RIG(block) {block, BLOCK_RIGHT_BIT}
#define FRO(block) {block, BLOCK_FRONT_BIT}
#define BAC(block) {block, BLOCK_BACK_BIT}
#define TAB(block) {block, BLOCK_TOP_BIT | BLOCK_BOTTOM_BIT}
#define SID(block) {block, BLOCK_FRONT_BIT | BLOCK_BACK_BIT | BLOCK_LEFT_BIT | BLOCK_RIGHT_BIT}
#define SWF(block) {block, BLOCK_BACK_BIT | BLOCK_LEFT_BIT | BLOCK_RIGHT_BIT}
#define AWF(block) {block, BLOCK_TOP_BIT | BLOCK_BOTTOM_BIT | BLOCK_LEFT_BIT | BLOCK_RIGHT_BIT | BLOCK_BACK_BIT}

//Maps location in texture atlas to block ID
static const BLOCK_TEXTURE texture_atlas[][16] =
{
    { TOP(BLOCK_GRASS), ALL(BLOCK_STONE), ALL(BLOCK_DIRT), SID(BLOCK_GRASS), ALL(BLOCK_PLANKS_NORMAL), NON, NON, ALL(BLOCK_WALL), ALL(BLOCK_TNT), TOP(BLOCK_TNT), BOT(BLOCK_TNT), NON, NON, NON, NON, NON },
    { NON, ALL(BLOCK_BEDROCK), ALL(BLOCK_SAND), ALL(BLOCK_COBBLESTONE), SID(BLOCK_WOOD), TAB(BLOCK_WOOD), ALL(BLOCK_IRON), ALL(BLOCK_GOLD), ALL(BLOCK_DIAMOND), NON, NON, NON, NON, NON, NON, NON },
    { ALL(BLOCK_GOLD_ORE), ALL(BLOCK_IRON_ORE), ALL(BLOCK_COAL_ORE), FRO(BLOCK_BOOKSHELF), NON, NON, NON, NON, NON, NON, NON, TAB(BLOCK_CRAFTING_TABLE), FRO(BLOCK_FURNACE), SWF(BLOCK_FURNACE), NON, NON },
    { ALL(BLOCK_SPONGE), ALL(BLOCK_GLASS), ALL(BLOCK_DIAMOND_ORE), ALL(BLOCK_REDSTONE_ORE), NON, ALL(BLOCK_LEAVES), NON, NON, NON, NON, NON, SID(BLOCK_CRAFTING_TABLE), FRO(BLOCK_CRAFTING_TABLE), NON, TOP(BLOCK_FURNACE), NON },
    { NON, NON, NON, NON, NON, TOP(BLOCK_CACTUS), SID(BLOCK_CACTUS), BOT(BLOCK_CACTUS), NON, NON, NON, NON, NON, NON, NON, NON },
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

// As special block breaking particles haven't been fully implemented yet
static const struct { int x, y; } special_block_texture_idx[BLOCK_SPECIAL_LAST - BLOCK_SPECIAL_START + 1] =
{
    {4, 0}, // Torch -> Planks
    {4, 3}, // Flower -> Leaves
    {11, 0}, // Spiderweb -> Spiderweb
    {9, 7}, // Cake -> Cake
    {14, 8}, // Mushroom -> Mushroom block
    {1, 6}, // Door -> Door bottom
    {13, 12}, // Water -> Water
    {13, 14}, // Lava -> Lava
    {15, 5}, // Wheat -> Wheat        // GAP REMOVED YOLO
    {3, 13}, // Redstone Lamp -> Lamp (off)
    {1, 0}, // Redstone Switch -> Stone
    {4, 10}, // Redstone Wire -> Redstone Wire
    {4, 0}, // Redstone Torch -> Planks
    {1, 0}, // Pressure Plate -> Stone
    {0, 4}, // Wool -> White Wool
};

TerrainAtlasEntry block_textures[BLOCK_NORMAL_LAST + 1][BLOCK_SIDE_LAST + 1];
TerrainQuadEntry quad_block_textures[BLOCK_NORMAL_LAST + 1][BLOCK_SIDE_LAST + 1], dual_block_textures[2][BLOCK_NORMAL_LAST + 1][BLOCK_SIDE_LAST + 1];
TerrainAtlasEntry terrain_atlas[16][16];
TerrainAtlasEntry special_block_textures[BLOCK_SPECIAL_LAST - BLOCK_SPECIAL_START + 1];

TEXTURE *terrain_current, *terrain_resized, *terrain_quad, *inv_selection_p, *door_preview;

//Some textures have a different color in different biomes. We have to make them green. Grey grass just looks so unhealty
static void makeColor(const RGB &color, TEXTURE &texture, const int x, const int y, const int w, const int h)
{
    for(int x1 = x; x1 < w + x; x1++)
        for(int y1 = y; y1 < h + y; y1++)
        {
            RGB grey = rgbColor(texture.bitmap[x1 + y1*texture.width]);
            grey.r *= color.r;
            grey.g *= color.g;
            grey.b *= color.b;
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

    //Give grass and leaves color
    const RGB green = { 0.5f, 0.8f, 0.3f };
    makeColor(green, *terrain_current, 0, 0, field_width, field_height);
    makeColor(green, *terrain_current, 5 * field_width, 3 * field_height, field_width, field_height);
    makeColor(green, *terrain_current, 4 * field_width, 3 * field_height, field_width, field_height);

    //Also redstone
    drawTexture(*terrain_current, *terrain_current, 4 * field_width, 10 * field_height, field_width, field_height, 4 * field_width, 11 * field_height, field_width, field_height);
    drawTexture(*terrain_current, *terrain_current, 5 * field_width, 10 * field_height, field_width, field_height, 5 * field_width, 11 * field_height, field_width, field_height);
    const RGB black = { 0.3f, 0.2f, 0.2f };
    makeColor(black, *terrain_current, 4 * field_width, 10 * field_height, field_width, field_height);
    makeColor(black, *terrain_current, 5 * field_width, 10 * field_height, field_width, field_height);
    const RGB red = { 1.0f, 0.2f, 0.2f };
    makeColor(red, *terrain_current, 4 * field_width, 11 * field_height, field_width, field_height);
    makeColor(red, *terrain_current, 5 * field_width, 11 * field_height, field_width, field_height);

    //And redstone switches
    drawTexture(*terrain_current, *terrain_current, 0 * field_width, 6 * field_height, field_width, field_height, 10 * field_width, 15 * field_height, field_width, field_height);
    const RGB red_tint = { 1.0f, 0.8f, 0.8f };
    makeColor(red_tint, *terrain_current, 10 * field_width, 15 * field_height, field_width, field_height);

    if(terrain_current->width == 384 && terrain_current->height == 384)
        terrain_resized = terrain_current;
    else
        terrain_resized = resizeTexture(*terrain_current, 384, 384);

    for(int y = 0; y < fields_y; y++)
        for(int x = 0; x < fields_x; x++)
        {
            //+1 and -2 to work around GLFix inaccuracies resulting in rounding errors
            TerrainAtlasEntry tea = terrain_atlas[x][y] = {textureArea(x * field_width + 1, y * field_height + 1, field_width - 2, field_height - 2),
                                                            textureArea(x * 24, y * 24, 24, 24) };

            BLOCK_TEXTURE bt = texture_atlas[y][x];
            if(bt.sides == 0)
                continue;

            if(bt.sides & BLOCK_BOTTOM_BIT)
                block_textures[bt.block][BLOCK_BOTTOM] = tea;
            if(bt.sides & BLOCK_TOP_BIT)
                block_textures[bt.block][BLOCK_TOP] = tea;
            if(bt.sides & BLOCK_LEFT_BIT)
                block_textures[bt.block][BLOCK_LEFT] = tea;
            if(bt.sides & BLOCK_RIGHT_BIT)
                block_textures[bt.block][BLOCK_RIGHT] = tea;
            if(bt.sides & BLOCK_FRONT_BIT)
                block_textures[bt.block][BLOCK_FRONT] = tea;
            if(bt.sides & BLOCK_BACK_BIT)
                block_textures[bt.block][BLOCK_BACK] = tea;
        }

    //Slight hack, you can't assign a texture to multiple blocks
    block_textures[BLOCK_GRASS][BLOCK_BOTTOM] = block_textures[BLOCK_DIRT][BLOCK_BOTTOM];

    // Assign special_block_textures based on special_block_texture_idx
    for(unsigned int i = 0; i < sizeof(special_block_texture_idx)/sizeof(*special_block_texture_idx); i++)
    {
        auto &idx = special_block_texture_idx[i];
        special_block_textures[i] = terrain_atlas[idx.x][idx.y];
    }

    //Prerender four times the same texture to speed up drawing, see terrain.h
    const BLOCK_TEXTURE quad_textures[] = { ALL(BLOCK_DIRT), SID(BLOCK_GRASS), TOP(BLOCK_GRASS), ALL(BLOCK_STONE), ALL(BLOCK_SAND), SID(BLOCK_WOOD), ALL(BLOCK_PLANKS_NORMAL), ALL(BLOCK_LEAVES) };
    terrain_quad = newTexture(field_width * 2 * (sizeof(quad_textures)/sizeof(*quad_textures)), field_height * 2);

    for(BLOCK b = 0; b <= BLOCK_NORMAL_LAST; b++)
        for(uint8_t s = 0; s <= BLOCK_SIDE_LAST; s++)
            quad_block_textures[b][s].has_quad = false;

    unsigned int x = 0;
    for(BLOCK_TEXTURE bt : quad_textures)
    {
        TextureAtlasEntry *tae = nullptr;

        if(bt.sides & BLOCK_BOTTOM_BIT)
            tae = &block_textures[bt.block][BLOCK_BOTTOM].current;
        if(bt.sides & BLOCK_TOP_BIT)
            tae = &block_textures[bt.block][BLOCK_TOP].current;
        if(bt.sides & BLOCK_LEFT_BIT)
            tae = &block_textures[bt.block][BLOCK_LEFT].current;
        if(bt.sides & BLOCK_RIGHT_BIT)
            tae = &block_textures[bt.block][BLOCK_RIGHT].current;
        if(bt.sides & BLOCK_FRONT_BIT)
            tae = &block_textures[bt.block][BLOCK_FRONT].current;
        if(bt.sides & BLOCK_BACK_BIT)
            tae = &block_textures[bt.block][BLOCK_BACK].current;

        if(!tae)
        {
            printf("Block %d has no texture!\n", bt.block);
            continue;
        }

        //- 1 to reverse the workaround above. Yes, I hate myself for this.
        drawTexture(*terrain_current, *terrain_quad, tae->left - 1, tae->top - 1, field_width, field_height, x, 0, field_width, field_height);
        drawTexture(*terrain_current, *terrain_quad, tae->left - 1, tae->top - 1, field_width, field_height, x + field_width, 0, field_width, field_height);
        drawTexture(*terrain_current, *terrain_quad, tae->left - 1, tae->top - 1, field_width, field_height, x+ field_width, field_height, field_width, field_height);
        drawTexture(*terrain_current, *terrain_quad, tae->left - 1, tae->top - 1, field_width, field_height, x, field_height, field_width, field_height);

        //Get an average color of the block
        RGB sum;
        for(unsigned int tex_x = tae->left - 1; tex_x <= tae->right; ++tex_x)
            for(unsigned int tex_y = tae->top - 1; tex_y <= tae->bottom; ++tex_y)
            {
                RGB rgb = rgbColor(terrain_current->bitmap[tex_x + tex_y*terrain_current->width]);
                sum.r += rgb.r;
                sum.g += rgb.g;
                sum.b += rgb.b;
            }

        int pixels = field_width * field_height;
        sum.r /= pixels;
        sum.g /= pixels;
        sum.b /= pixels;

        const COLOR darker = colorRGB(sum.r / GLFix(1.5f), sum.g / GLFix(1.5f), sum.b / GLFix(1.5f));

        //And add the workaround here again..
        TerrainQuadEntry tqe = { true, textureArea(x + 1, 1, field_width * 2 - 2, field_height * 2 - 2), colorRGB(sum), darker };

        if(bt.sides & BLOCK_BOTTOM_BIT)
            quad_block_textures[bt.block][BLOCK_BOTTOM] = tqe;
        if(bt.sides & BLOCK_TOP_BIT)
            quad_block_textures[bt.block][BLOCK_TOP] = tqe;
        if(bt.sides & BLOCK_LEFT_BIT)
            quad_block_textures[bt.block][BLOCK_LEFT] = tqe;
        if(bt.sides & BLOCK_RIGHT_BIT)
            quad_block_textures[bt.block][BLOCK_RIGHT] = tqe;
        if(bt.sides & BLOCK_FRONT_BIT)
            quad_block_textures[bt.block][BLOCK_FRONT] = tqe;
        if(bt.sides & BLOCK_BACK_BIT)
            quad_block_textures[bt.block][BLOCK_BACK] = tqe;

        x += field_width * 2;
    }

    //Part 2 of the hack above
    quad_block_textures[BLOCK_GRASS][BLOCK_BOTTOM] = quad_block_textures[BLOCK_DIRT][BLOCK_BOTTOM];

    if(lcd_type() == SCR_320x240_4)
    {
        greyscaleTexture(*terrain_current);
        greyscaleTexture(*terrain_resized);
        greyscaleTexture(*terrain_quad);
    }

    //Make the texture available to others for sharing
    inv_selection_p = &inv_selection;

    door_preview = newTexture(16, 32);
    TextureAtlasEntry door = terrain_atlas[1][5].current;
    door.bottom += door.bottom - door.top; //Double height
    drawTexture(*terrain_current, *door_preview, door.left, door.top, door.right - door.left, door.bottom - door.top, 0, 0, 16, 32);
}

void terrainUninit()
{
    if(terrain_resized != terrain_current)
        deleteTexture(terrain_resized);

    if(terrain_current != &terrain)
        deleteTexture(terrain_current);

    deleteTexture(terrain_quad);

    deleteTexture(door_preview);
}
