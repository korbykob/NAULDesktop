#include <windows.h>
#include <desktop.h>
#include <drawing.h>
#include <taskbar.h>
#include <inputs.h>
#include <allocator.h>
#include <scheduler.h>
#include <ipc.h>
#include <mem.h>

WindowElement* windows = 0;
WindowElement* taskbarWindows = 0;

Window* createWindow(uint32_t width, uint32_t height)
{
    WindowElement* element = allocate(sizeof(WindowElement));
    ((WindowElement*)windows->prev)->next = element;
    element->next = windows;
    element->prev = windows->prev;
    windows->prev = element;
    element->window = allocate(sizeof(Window));
    element->window->width = width + 20;
    element->window->height = height + 57;
    element->window->x = display.width / 2 - element->window->width / 2;
    element->window->y = display.height / 2 - element->window->height / 2;
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
    focus = element->window;
    WindowElement* taskbarElement = allocate(sizeof(WindowElement));
    ((WindowElement*)taskbarWindows->prev)->next = taskbarElement;
    taskbarElement->next = taskbarWindows;
    taskbarElement->prev = taskbarWindows->prev;
    taskbarWindows->prev = taskbarElement;
    taskbarElement->window = element->window;
    return element->window;
}

void destroyWindow(Window* window)
{
    if (focus == window)
    {
        focus = 0;
        dragging = 0;
        mouseFree = true;
    }
    WindowElement* taskbarElement = taskbarWindows;
    while (taskbarElement->window != window)
    {
        taskbarElement = taskbarElement->next;
    }
    ((WindowElement*)taskbarElement->prev)->next = taskbarElement->next;
    ((WindowElement*)taskbarElement->next)->prev = taskbarElement->prev;
    unallocate(taskbarElement);
    WindowElement* element = windows;
    while (element->window != window)
    {
        element = element->next;
    }
    ((WindowElement*)element->prev)->next = element->next;
    ((WindowElement*)element->next)->prev = element->prev;
    unallocate(element);
    unallocate(window->buffer);
    unallocate(window);
}

uint64_t listener(uint64_t code, uint64_t arg1, uint64_t arg2, uint64_t arg3)
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
    windows = allocate(sizeof(WindowElement));
    windows->next = windows;
    windows->prev = windows;
    windows->window = 0;
    registerListener(listener, IPC_NAME);
}

void cleanupWindows()
{
    unregisterListener(listener);
    WindowElement* element = windows;
    while (true)
    {
        if (element->window)
        {
            element->window->events[element->window->eventsHead].type = CLOSE_EVENT;
            element->window->eventsHead++;
        }
        element = element->next;
        if (element == windows)
        {
            break;
        }
    }
    while (windows->next != windows)
    {
        yieldThread();
    }
}
