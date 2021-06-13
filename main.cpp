#include <libndls.h>
#include <unistd.h>

#include "gl.h"
#include "terrain.h"
#include "worldtask.h"

#include "textures/loading.h"

int main(int argc, char *argv[])
{
    #ifdef _TINSPIRE
        //Sometimes there's a clock on screen, switch that off
        __asm__ volatile("mrs r0, cpsr;"
                        "orr r0, r0, #0x80;"
                        "msr cpsr_c, r0;" ::: "r0");
    #endif

    nglInit();
    if(lcd_type() == SCR_320x240_4)
        greyscaleTexture(loading);
    nglSetBuffer(loading.bitmap);
    nglDisplay();

    //Early exit #1
    //(Task::keyPressed can only be used after initializeGlobals)
    if(isKeyPressed(KEY_NSPIRE_ESC))
    {
        nglUninit();
        return 0;
    }

    terrainInit("/documents/ndless/crafti.ppm.tns");
    glBindTexture(terrain_current);

    glLoadIdentity();

    //Early exit #2
    if(isKeyPressed(KEY_NSPIRE_ESC))
    {
        terrainUninit();
        nglUninit();

        return 0;
    }

    //If crafti has been started by the file extension association, use the first argument as savefile path
    Task::initializeGlobals(argc > 1 ? argv[1] : "/documents/ndless/crafti.map.tns");

    if(Task::load())
        world_task.setMessage("World loaded.");
    else
        world_task.setMessage("World failed to load.");

    //Start with WorldTask as current task
    world_task.makeCurrent();

    while(Task::running)
    {
        //Reset "loading" message
        drawLoadingtext(-1);

        Task::current_task->render();

        nglDisplay();

        Task::current_task->logic();
    }

    Task::deinitializeGlobals();

    nglUninit();

    terrainUninit();

    return 0;
}
