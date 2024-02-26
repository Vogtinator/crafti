#include "task.h"
#include <zlib.h>

#include "texturetools.h"
#include "blocklisttask.h"
#include "worldtask.h"
#include "settingstask.h"
#include "inventory.h"

#ifndef _TINSPIRE
#include <SDL/SDL_events.h>
#include <unistd.h>
touchpad_report_t tpad_report = {};
static bool key_pressed[256] = {};
#endif

//The values have to stay somewhere
Task *Task::current_task;
bool Task::key_held_down, Task::running, Task::background_saved, Task::has_touchpad, Task::keys_inverted;
TEXTURE *Task::screen, *Task::background;
const char *Task::savefile;

void Task::makeCurrent()
{
    current_task = this;
}

bool Task::keyPressed(const t_key &key)
{
    #ifdef _TINSPIRE
        if(has_touchpad)
        {
            if(key.tpad_arrow != TPAD_ARROW_NONE)
                return touchpad_arrow_pressed(key.tpad_arrow);
            else
                return !(*reinterpret_cast<volatile uint16_t*>(0x900E0000 + key.tpad_row) & key.tpad_col) == keys_inverted;
        }
        else
            return (*reinterpret_cast<volatile uint16_t*>(0x900E0000 + key.row) & key.col) == 0;
    #else
        if(key < sizeof(key_pressed)/sizeof(*key_pressed))
            return key_pressed[key];
        return false;
    #endif
}

void Task::handleEvents()
{
#ifndef _TINSPIRE
    SDL_Event event;

    usleep(30000);

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            if(event.key.keysym.sym < sizeof(key_pressed)/sizeof(*key_pressed))
                key_pressed[event.key.keysym.sym] = true;
            break;

        case SDL_KEYUP:
            if(event.key.keysym.sym < sizeof(key_pressed)/sizeof(*key_pressed))
                key_pressed[event.key.keysym.sym] = false;
            break;

        case SDL_MOUSEMOTION:
            tpad_report.contact = true;
            tpad_report.x = event.motion.x * 25;
            tpad_report.y = event.motion.y * -25;
            break;

        case SDL_QUIT:
            running = false;
            break;

        default:
            break;
        }
    }
#endif
}

void Task::initializeGlobals(const char *savefile)
{
    running = true;

    screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    nglSetBuffer(screen->bitmap);

    has_touchpad = is_touchpad;
    keys_inverted = is_classic;

    background = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    background_saved = false;

    Task::savefile = savefile;
}

void Task::deinitializeGlobals()
{
    deleteTexture(screen);
    deleteTexture(background);
}

void Task::saveBackground()
{
    copyTexture(*screen, *background);

    background_saved = true;
}

void Task::drawBackground()
{
    copyTexture(*background, *screen);
}

/* Version 2: First in git
 * Version 3 (31d5ee3a): Blocks are stored as 16bit BLOCK_WDATA
 * Version 4 (1ebc685a): Add inventory
 * Version 5 (710e7269): Add settings
 * Version 6 (d52f3992): BLOCK_SIZE changed from 120 to 128,
 *                       gzip compression introduced shortly afterwards
 */
static constexpr int savefile_version = 6;

#define LOAD_FROM_FILE(var) if(gzfread(&var, sizeof(var), 1, file) != 1) { gzclose(file); return false; }
#define SAVE_TO_FILE(var) if(gzfwrite(&var, sizeof(var), 1, file) != 1) { gzclose(file); return false; }

bool Task::load()
{
    // Versions before 6 (and 6 for a short time) were uncompressed,
    // but gzopen detects and handles uncompressed files transparently.
    // Previous versions read the gzip magic as savefile version and bail out.
    gzFile file = gzopen(savefile, "rb");
    if(!file)
        return false;

    int version;
    LOAD_FROM_FILE(version);

    static_assert(savefile_version == 6, "Adjust loading code for backward compatibility");

    if(version < 4 || version > 6)
    {
        printf("Save file version %d not supported!\n", version);
        gzclose(file);
        return false;
    }

    if(!settings_task.loadFromFile(file, version))
    {
        gzclose(file);
        return false;
    }

    LOAD_FROM_FILE(current_inventory.entries)
    LOAD_FROM_FILE(world_task.xr)
    LOAD_FROM_FILE(world_task.yr)
    LOAD_FROM_FILE(world_task.x)
    LOAD_FROM_FILE(world_task.y)
    LOAD_FROM_FILE(world_task.z)
    // Previous versions used BLOCK_SIZE 120
    if(version < 6)
    {
        world_task.x = world_task.x * BLOCK_SIZE / 120;
        world_task.y = world_task.y * BLOCK_SIZE / 120;
        world_task.z = world_task.z * BLOCK_SIZE / 120;
    }

    LOAD_FROM_FILE(current_inventory.current_slot)

    LOAD_FROM_FILE(block_list_task.current_selection)

    const bool ret = world.loadFromFile(file);

    gzclose(file);

    world.setPosition(world_task.x, world_task.y, world_task.z);

    return ret;
}

bool Task::save()
{
    gzFile file = gzopen(savefile, "wb");
    if(!file)
        return false;

    SAVE_TO_FILE(savefile_version)
    if(!settings_task.saveToFile(file))
    {
        gzclose(file);
        return false;
    }
    SAVE_TO_FILE(current_inventory.entries)
    SAVE_TO_FILE(world_task.xr)
    SAVE_TO_FILE(world_task.yr)
    SAVE_TO_FILE(world_task.x)
    SAVE_TO_FILE(world_task.y)
    SAVE_TO_FILE(world_task.z)
    SAVE_TO_FILE(current_inventory.current_slot)
    SAVE_TO_FILE(block_list_task.current_selection)

    const bool ret = world.saveToFile(file);

    gzclose(file);

    return ret;
}
