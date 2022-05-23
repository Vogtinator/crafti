#include "menutask.h"

#include "texturetools.h"
#include "worldtask.h"
#include "helptask.h"
#include "settingstask.h"

#include "textures/menu.h"
#include "textures/selection.h"

MenuTask menu_task;

MenuTask::MenuTask()
{
     menu_with_selection = newTexture(menu.width, menu.height);
}

MenuTask::~MenuTask()
{
    deleteTexture(menu_with_selection);
}

void MenuTask::makeCurrent()
{
    menu_open = true;
    menu_width_visible = 0;
    menu_selected_item = SAVE_WORLD;

    if(!background_saved)
        saveBackground();

    Task::makeCurrent();
}

void MenuTask::render()
{
    drawBackground();

    copyTexture(menu, *menu_with_selection);

    const int selection_y[MENU_ITEM_MAX] = { 14, 36, 63, 88, 112, 136 };

    drawTexture(selection, *menu_with_selection, 0, 0, selection.width, selection.height, 23, selection_y[menu_selected_item], selection.width, selection.height);

    drawTexture(*menu_with_selection, *screen, 0, 0, menu_width_visible, menu_with_selection->height, SCREEN_WIDTH - menu_width_visible, 0, menu_width_visible, menu_with_selection->height);
}

void MenuTask::logic()
{
    //Slide menu
    if(menu_open && static_cast<unsigned int>(menu_width_visible) < menu_with_selection->width)
        menu_width_visible += 10;
    else if(!menu_open && menu_width_visible > 0)
        menu_width_visible -= 10;

    if(menu_width_visible < 0)
        menu_width_visible = 0;
    else if(static_cast<unsigned int>(menu_width_visible) > menu.width)
        menu_width_visible = menu.width;

    //Wait for the menu to be closed, then set the current task
    if(!menu_open && menu_width_visible == 0)
    {
        switch(menu_selected_item)
        {
        case HELP:
            help_task.makeCurrent();
            break;
        case SETTINGS:
            settings_task.makeCurrent();
            world_task.setMessage("Settings applied.");
            break;
        default:
            world_task.makeCurrent();
            break;
        }

        return;
    }

    if(key_held_down)
        key_held_down = keyPressed(KEY_NSPIRE_CLICK) || keyPressed(KEY_NSPIRE_UP) || keyPressed(KEY_NSPIRE_DOWN) || keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_5) || keyPressed(KEY_NSPIRE_MENU) || keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_W) || keyPressed(KEY_NSPIRE_S) || keyPressed(KEY_NSPIRE_ENTER);
    else if(keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_UP) || keyPressed(KEY_NSPIRE_W))
    {
        --menu_selected_item;
        if(menu_selected_item < 0)
            menu_selected_item = MENU_ITEM_MAX - 1;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_DOWN) || keyPressed(KEY_NSPIRE_S))
    {
        ++menu_selected_item;
        if(menu_selected_item == MENU_ITEM_MAX)
            menu_selected_item = 0;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_5) || keyPressed(KEY_NSPIRE_CLICK) || keyPressed(KEY_NSPIRE_ENTER))
    {
        switch(menu_selected_item)
        {
        case NEW_WORLD:
            world_task.resetWorld();
            break;

        case LOAD_WORLD:
            if(load())
                world_task.setMessage("World loaded.");
            else
                world_task.setMessage("World failed to load.");
            break;

        case SAVE_WORLD:
            if(save())
                world_task.setMessage("World saved.");
            else
                world_task.setMessage("Failed to save world.");
            break;

        case EXIT:
            running = false;
            break;

        case HELP:
        case SETTINGS:
            //Handled above, at the start of this function
            break;
        }

        menu_open = false;
        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_MENU) || keyPressed(KEY_NSPIRE_ESC))
    {
        menu_open = false;
        key_held_down = true;
    }
}
