#ifndef TASK_H
#define TASK_H

#include "gl.h"

#include <libndls.h>

class Task
{
public:
    virtual ~Task() {}

    virtual void render() = 0;
    virtual void logic() = 0;

    virtual void makeCurrent();

    static bool keyPressed(const t_key &key);

    static void initializeGlobals(const char *savefile);
    static void deinitializeGlobals();

    //All tasks share these values

    //Pointer to the current running task
    static Task *current_task;
    //Whether a key is being held down
    static bool key_held_down;
    //Whether the machine has a touchpad
    static bool has_touchpad, keys_inverted;
    //The application will exit if this is false
    static bool running;
    //The buffer nGL renders to
    static TEXTURE *screen;
    //Some tasks draw the last frame of world_task as a background
    static TEXTURE *background;
    static bool background_saved;
    static void saveBackground();
    static void drawBackground();
    //Saving and loading
    static bool load();
    static bool save();
    static const char *savefile;
};

#endif // TASK_H
