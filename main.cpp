#include <nucleus.h>
#include <libndls.h>
#include <unistd.h>

#include "gl.h"
#include "terrain.h"
#include "worldtask.h"

#include "textures/loading.h"

int main(int argc, char *argv[])
{
    //Sometimes there's a clock on screen, switch that off
    __asm__ volatile("mrs r0, cpsr;"
                    "orr r0, r0, #0x80;"
                    "msr cpsr_c, r0;" ::: "r0");

    if(lcd_isincolor())
    {
        std::copy(loading.bitmap, loading.bitmap + SCREEN_HEIGHT*SCREEN_WIDTH, reinterpret_cast<COLOR*>(SCREEN_BASE_ADDRESS));
        nglInit();
    }
    else //Monochrome calcs don't like colored stuff in the framebuffer, so let nGL handle that
    {
        nglInit();
        greyscaleTexture(loading);
        nglSetBuffer(loading.bitmap);
        nglDisplay();
    }

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
        puts("Loaded world.");
    else
        puts("Failed to load world!");

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
