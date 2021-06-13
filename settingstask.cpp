#include "settingstask.h"

#include "font.h"
#include "texturetools.h"
#include "worldtask.h"

#include "textures/selection.h"

SettingsTask settings_task;

const char *leaves_values[] = {
    "Opaque",
    "Transparent"
};

const char *speed_values[] = {
    "Slow",
    "Normal",
    "Fast"
};

const char *fastmode_values[] = {
    "Off",
    "On"
};

const char *world_static_values[] = {
    "Static (no ticks)",
    "Dynamic"
};

SettingsTask::SettingsTask()
{
    //Must have the same order as the "Settings" enum
    settings.push_back({"Leaves", leaves_values, 2, 0, 0, 1});
    settings.push_back({"Speed", speed_values, 3, 1, 0, 1});
    settings.push_back({"Distance", nullptr, 10, 2, 1, 1});
    settings.push_back({"Fast mode", fastmode_values, 2, 0, 0, 1});
    settings.push_back({"Near plane", nullptr, 500, 256, 120, 16});
    settings.push_back({"World", world_static_values, 2, 1, 0, 1});
    settings.push_back({"Show FPS", fastmode_values, 2, 0, 0, 1});

    background = newTexture(background_width, background_height, 0, false);
}

SettingsTask::~SettingsTask()
{
    deleteTexture(background);
}

void SettingsTask::makeCurrent()
{
    if(!background_saved)
        saveBackground();

    settings[DISTANCE].current_value = world.fieldOfView();

    changed_something = false;

    Task::makeCurrent();
}

void SettingsTask::render()
{
    drawBackground();

    const unsigned int x = (SCREEN_WIDTH - background->width) / 2;
    unsigned int y = (SCREEN_HEIGHT - background->height) / 2;
    drawTextureOverlay(*background, 0, 0, *screen, x, y, background->width, background->height);
    drawString("Settings", 0xFFFF, *screen, x, y - fontHeight());

    y += 8;

    for(unsigned int i = 0; i < settings.size(); ++i)
    {
        SettingsEntry &e = settings[i];

        if(i == current_selection)
            drawTexture(selection, *screen, 0, 0, selection.width, selection.height, x + 5, y, selection.width, selection.height);

        drawString(e.name, 0xFFFF, *screen, x + selection.width + 10, y);

        //Print the numeric value
        if(e.values == nullptr)
        {
            char number[10];
            //Somehow sniprintf isn't available...
            sprintf(number, "%d", e.current_value);
            drawString(number, 0xFFFF, *screen, x + 100, y);
        }
        else
            drawString(e.values[e.current_value], 0xFFFF, *screen, x + 100, y);

        y += fontHeight() + 5;
    }

}

void SettingsTask::logic()
{
    if(key_held_down)
        key_held_down = keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_UP) || keyPressed(KEY_NSPIRE_DOWN) || keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_LEFT) || keyPressed(KEY_NSPIRE_4) || keyPressed(KEY_NSPIRE_RIGHT) || keyPressed(KEY_NSPIRE_6);
    else if(keyPressed(KEY_NSPIRE_ESC))
    {
        world_task.makeCurrent();

        if(changed_something)
        {
            world.setDirty();
            world.setFieldOfView(settings[DISTANCE].current_value);

            nglSetNearPlane(settings[NEARPLANE_Z].current_value);
        }

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_UP) || keyPressed(KEY_NSPIRE_8))
    {
        if(current_selection == 0)
            current_selection = settings.size() - 1;
        else
            --current_selection;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_DOWN) || keyPressed(KEY_NSPIRE_2))
    {
        ++current_selection;
        if(current_selection >= settings.size())
            current_selection = 0;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_LEFT) || keyPressed(KEY_NSPIRE_4))
    {
        SettingsEntry &entry = settings[current_selection];
        if(entry.current_value < entry.min_value + entry.step)
            entry.current_value = entry.values_count - 1;
        else
            entry.current_value -= entry.step;

        changed_something = true;

        key_held_down = true;
    }
    else if(keyPressed(KEY_NSPIRE_RIGHT) || keyPressed(KEY_NSPIRE_6))
    {
        SettingsEntry &entry = settings[current_selection];
        entry.current_value += entry.step;
        if(entry.current_value >= entry.values_count)
            entry.current_value = entry.min_value;

        changed_something = true;

        key_held_down = true;
    }
}

unsigned int SettingsTask::getValue(unsigned int entry) const
{
    return settings[entry].current_value;
}

bool SettingsTask::loadFromFile(FILE *file)
{
    //World doesn't care about DISTANCE being saved and loaded here as well

    unsigned int size;
    if(fread(&size, sizeof(size), 1, file) != 1)
        return false;

    //For backwards compatibility
    if(size != settings.size())
        return fseek(file, size * sizeof(unsigned int), SEEK_CUR) == 0;

    for(unsigned int i = 0; i < size; ++i)
    {
        if(fread(&settings[i].current_value, sizeof(unsigned int), 1, file) != 1)
            return false;
    }

    world.setDirty();

    nglSetNearPlane(settings[NEARPLANE_Z].current_value);

    return true;
}

bool SettingsTask::saveToFile(FILE *file)
{
    unsigned int size = settings.size();
    if(fwrite(&size, sizeof(size), 1, file) != 1)
        return false;

    for(unsigned int i = 0; i < size; ++i)
    {
        if(fwrite(&settings[i].current_value, sizeof(unsigned int), 1, file) != 1)
            return false;
    }

    return true;
}
