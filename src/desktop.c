#include <desktop.h>
#include <taskbar.h>
#include <clock.h>
#include <drawing.h>
#include <inputs.h>
#include <windows.h>
#include <processes.h>
#include <ipc.h>
#include <tty.h>
#include <hpet.h>
#include <cpu.h>

#define FPS_CAP 120

void exitDesktop()
{
    cleanupWindows();
    cleanupInputs();
    cleanupDrawing();
    cleanupClock();
    cleanupTaskbar();
    quit();
}

void entry()
{
    if (checkListener(IPC_NAME))
    {
        write("Desktop is already running\n");
        quit();
    }
    initTaskbar();
    initClock();
    initDrawing();
    initInputs();
    initWindows();
    draw();
    uint64_t last = getFemtoseconds();
    while (true)
    {
        processInputs();
        yieldThread();
        uint64_t femtoseconds = getFemtoseconds();
        if (femtoseconds - last >= FEMTOSECONDS_PER_SECOND / FPS_CAP)
        {
            last = femtoseconds;
            draw();
        }
    }
}
