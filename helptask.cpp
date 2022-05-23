#include "helptask.h"

#include "font.h"
#include "worldtask.h"

HelpTask help_task;

HelpTask::HelpTask()
{
    background = newTexture(background_width, background_height, 0, false);
}

HelpTask::~HelpTask()
{
    deleteTexture(background);
}

void HelpTask::makeCurrent()
{
    if(!background_saved)
        saveBackground();

    Task::makeCurrent();
}

void HelpTask::render()
{
    drawBackground();

    const unsigned int x = (SCREEN_WIDTH - background->width) / 2;
    const unsigned int y = (SCREEN_HEIGHT - background->height) / 2;
    drawTextureOverlay(*background, 0, 0, *screen, x, y, background->width, background->height);
    drawString("Help for Crafti v1.3", 0xFFFF, *screen, x, y - fontHeight());

    drawString("8-4-6-2: Walk around\t5: Jump\n"
               "7: Put block down   \t9: Destroy block\n"
               "1-3: Change inventory slot\n"
               "ESC: Save & Exit\n"
               ".: Open list of blocks\n"
               "    5: Change block in inventory\n"
               "    . or ESC: Close list of blocks\n"
               "Menu: Open menu\n"
               "    2-8: Move cursor\t5: Select\n"
               "Ctrl+.: Take screenshot\n"
               "\n"
               "Programmed by Fabian Vogt\n"
               "Textures from PureBDcraft by https://bdcraft.net", 0xFFFF, *screen, x + 10, y + 8);
}

void HelpTask::logic()
{
    if(key_held_down)
        key_held_down = keyPressed(KEY_NSPIRE_ESC);
    else if(keyPressed(KEY_NSPIRE_ESC))
    {
        world_task.makeCurrent();

        key_held_down = true;
    }
}
