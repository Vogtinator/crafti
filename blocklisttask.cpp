#include "blocklisttask.h"

#include <algorithm>

#include "blockrenderer.h"
#include "font.h"
#include "inventory.h"
#include "terrain.h"
#include "texturetools.h"
#include "worldtask.h"

BlockListTask block_list_task;

static const BLOCK_WDATA user_selectable[] = {
    BLOCK_STONE,
    BLOCK_COBBLESTONE,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_SAND,
    BLOCK_WOOD,
    BLOCK_LEAVES,
    BLOCK_PLANKS_NORMAL,
    BLOCK_PLANKS_DARK,
    BLOCK_PLANKS_BRIGHT,
    BLOCK_WALL,
    BLOCK_GLASS,
    BLOCK_DOOR,
    BLOCK_COAL_ORE,
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_DIAMOND_ORE,
    BLOCK_REDSTONE_ORE,
    BLOCK_IRON,
    BLOCK_GOLD,
    BLOCK_DIAMOND,
    BLOCK_GLOWSTONE,
    BLOCK_NETHERRACK,
    BLOCK_TNT,
    BLOCK_SPONGE,
    BLOCK_FURNACE,
    BLOCK_CRAFTING_TABLE,
    BLOCK_BOOKSHELF,
    BLOCK_PUMPKIN,
    getBLOCKWDATA(BLOCK_WATER, RANGE_WATER),
    getBLOCKWDATA(BLOCK_LAVA, RANGE_LAVA),
    getBLOCKWDATA(BLOCK_FLOWER, 0),
    getBLOCKWDATA(BLOCK_FLOWER, 1),
    getBLOCKWDATA(BLOCK_MUSHROOM, 0),
    getBLOCKWDATA(BLOCK_MUSHROOM, 1),
    getBLOCKWDATA(BLOCK_WHEAT, 0),
    BLOCK_SPIDERWEB,
    BLOCK_TORCH,
    BLOCK_CAKE,
    BLOCK_REDSTONE_LAMP,
    BLOCK_REDSTONE_SWITCH,
    BLOCK_PRESSURE_PLATE,
    BLOCK_REDSTONE_WIRE,
    BLOCK_REDSTONE_TORCH
};

//The values have to stay somewhere
unsigned int BlockListTask::blocklist_top;
//Black texture as background
TEXTURE *BlockListTask::blocklist_background;

constexpr int user_selectable_count = sizeof(user_selectable)/sizeof(*user_selectable);

BlockListTask::BlockListTask()
{
    blocklist_top = (SCREEN_HEIGHT - blocklist_height - current_inventory.height()) / 2;

    static_assert(field_width * fields_x <= SCREEN_WIDTH, "fields_x too high");
    static_assert(fields_x * fields_y >= sizeof(user_selectable)/sizeof(*user_selectable), "Not enough fields");
    if(blocklist_height + current_inventory.height() > SCREEN_WIDTH)
        printf("fields_y too high\n");

    blocklist_background = newTexture(blocklist_width, blocklist_height, 0, false);
}

BlockListTask::~BlockListTask()
{
    deleteTexture(blocklist_background);
}

void BlockListTask::makeCurrent()
{
    if(!background_saved)
        saveBackground();

    Task::makeCurrent();
}

void BlockListTask::render()
{
    drawBackground();

    drawTextureOverlay(*blocklist_background, 0, 0, *screen, blocklist_left, blocklist_top, blocklist_background->width, blocklist_background->height);

    int block_nr = 0;
    int screen_x, screen_y = blocklist_top + pad_y;
    for(int y = 0; y < fields_y; y++, screen_y += field_height)
    {
        screen_x = blocklist_left + pad_x;
        for(int x = 0; x < fields_x; x++, screen_x += field_width)
        {
            //BLOCK_DOOR is twice as high, so center it manually
            if(getBLOCK(user_selectable[block_nr]) == BLOCK_DOOR)
                global_block_renderer.drawPreview(user_selectable[block_nr], *screen, screen_x + pad_x, screen_y + pad_y_door);
            else
                global_block_renderer.drawPreview(user_selectable[block_nr], *screen, screen_x + pad_y, screen_y + pad_y);

            //Again, use the glass texture as selection indicator
            if(block_nr == current_selection)
                drawTexture(*inv_selection_p, *screen, 0, 0, inv_selection_p->width, inv_selection_p->height, screen_x + pad_x - 11, screen_y + pad_y - 10, inv_selection_p->width, inv_selection_p->height);

            block_nr++;
            if(block_nr == user_selectable_count)
                goto end;
        }
    }

    end:

    current_inventory.draw(*screen);
    drawStringCenter(global_block_renderer.getName(user_selectable[current_selection]), 0xFFFF, *screen, SCREEN_WIDTH / 2, SCREEN_HEIGHT - current_inventory.height() - fontHeight());
}

void BlockListTask::logic()
{
    if(key_held_down)
        key_held_down = keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_4) || keyPressed(KEY_NSPIRE_6) || keyPressed(KEY_NSPIRE_1) || keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_5) || keyPressed(KEY_NSPIRE_UP) || keyPressed(KEY_NSPIRE_DOWN) || keyPressed(KEY_NSPIRE_LEFT) || keyPressed(KEY_NSPIRE_RIGHT)  || keyPressed(KEY_NSPIRE_CLICK);
    else if(keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_PERIOD))
    {
        world_task.makeCurrent();

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_DOWN))
    {
        current_selection += fields_x;
        if(current_selection >= user_selectable_count)
            current_selection %= fields_x;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_UP))
    {
        if(current_selection >= fields_x)
            current_selection -= fields_x;
        else
        {
            current_selection = ((user_selectable_count - 1) / fields_x) * fields_x + (current_selection % fields_x);
            if(current_selection >= user_selectable_count)
                current_selection -= fields_x;
        }

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_4) || keyPressed(KEY_NSPIRE_LEFT))
    {
        if(current_selection % fields_x == 0)
        {
            current_selection += fields_x - 1;
            if(current_selection >= user_selectable_count)
                current_selection = user_selectable_count - 1;
        }
        else
            current_selection--;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_6) || keyPressed(KEY_NSPIRE_RIGHT))
    {
        if(current_selection % fields_x != fields_x-1 && current_selection < user_selectable_count - 1)
            current_selection++;
        else
            current_selection -= current_selection % fields_x;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_1)) //Switch inventory slot
    {
        current_inventory.previousSlot();

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_3))
    {
        current_inventory.nextSlot();

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_5) || keyPressed(KEY_NSPIRE_CLICK))
    {
        current_inventory.currentSlot() = user_selectable[current_selection];

        key_held_down = true;
    }
}
