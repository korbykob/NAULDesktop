#include <windows.h>
#include <drawing.h>
#include <inputs.h>
#include <allocator.h>
#include <ipc.h>
#include <mem.h>
#include <cpu.h>

List windows = { &windows, &windows };
List taskbarWindows = { &taskbarWindows, &taskbarWindows };
bool windowsLock = false;

Window* createWindow(uint32_t width, uint32_t height)
{
    WindowElement* element = allocate(sizeof(WindowElement));
    element->window = allocate(sizeof(Window));
    element->window->width = width + 20;
    element->window->height = height + 57;
    element->window->x = display.width / 2 - element->window->width / 2;
    element->window->y = display.height / 2 - 16 - element->window->height / 2;
    element->window->minimised = false;
    element->window->lockMouse = false;
    element->window->bufferWidth = width;
    element->window->bufferHeight = height;
    element->window->buffer = allocate(width * height * sizeof(uint32_t));
    element->window->eventsHead = 0;
    element->window->eventsTail = 0;
    element->window->title[0] = '\0';
    setMemory32(element->window->buffer, 0, width * height);
    setMemory32(element->window->icon, 0, 24 * 24);
    WindowElement* taskbarElement = allocate(sizeof(WindowElement));
    taskbarElement->window = element->window;
    lock(&windowsLock);
    linkList(&windows, &element->header);
    linkList(&taskbarWindows, &taskbarElement->header);
    focus = element->window;
    unlock(&windowsLock);
    return element->window;
}

void destroyWindow(Window* window)
{
    lock(&windowsLock);
    if (focus == window)
    {
        focus = 0;
        dragging = 0;
        mouseFree = true;
    }
    WindowElement* taskbarElement = (WindowElement*)&taskbarWindows;
    while (iterateList(&taskbarWindows, (List**)&taskbarElement))
    {
        if (taskbarElement->window == window)
        {
            break;
        }
    }
    WindowElement* element = (WindowElement*)&windows;
    while (iterateList(&windows, (List**)&element))
    {
        if (element->window == window)
        {
            break;
        }
    }
    unlinkList(&taskbarElement->header);
    unlinkList(&element->header);
    unlock(&windowsLock);
    unallocate(taskbarElement);
    unallocate(element);
    unallocate(window->buffer);
    unallocate(window);
}

uint64_t listener(uint64_t code, uint64_t arg1, uint64_t arg2, __attribute__((unused)) uint64_t arg3)
{
    switch (code)
    {
        case CREATE_WINDOW:
            return (uint64_t)createWindow(arg1, arg2);
        case DESTROY_WINDOW:
            destroyWindow((Window*)arg1);
            break;
    }
    return 0;
}

void initWindows()
{
    registerListener(listener, IPC_NAME);
}

void cleanupWindows()
{
    WindowElement* element = (WindowElement*)&windows;
    while (iterateList(&windows, (List**)&element))
    {
        element->window->events[element->window->eventsHead].type = CLOSE_EVENT;
        element->window->eventsHead++;
    }
    unlock(&windowsLock);
    while (true)
    {
        lock(&windowsLock);
        uint64_t count = listLength(&windows);
        unlock(&windowsLock);
        if (count > 0)
        {
            yieldThread();
        }
        else
        {
            break;
        }
    }
    unregisterListener(listener);
}
