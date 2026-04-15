#include <desktop.h>
#include <taskbar.h>
#include <clock.h>
#include <drawing.h>
#include <inputs.h>
#include <windows.h>
#include <processes.h>
#include <ipc.h>
#include <tty.h>
#include <cpu.h>

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
    while (true)
    {
        processInputs();
        yieldThread();
        draw();
    }
}
