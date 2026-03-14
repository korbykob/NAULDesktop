#include <inputs.h>
#include <desktop.h>
#include <drawing.h>
#include <taskbar.h>
#include <keyboard.h>
#include <mouse.h>
#include <processes.h>

#define SUPER_KEY 91
#define checkOverlap(x, y, width, height) (mouseX >= x && mouseX < (x + width) && mouseY >= y && mouseY < (y + height))

KeyboardBuffer keyboardBuffer = { 0 };
MouseBuffer mouseBuffer = { 0 };
uint32_t mouseX = 0;
uint32_t mouseY = 0;
bool lastLeft = false;
bool lastRight = false;
Window* focus = 0;
Window* dragging = 0;
int64_t draggingX = 0;
int64_t draggingY = 0;
bool mouseFree = true;

void initInputs()
{
    registerKeyboard(&keyboardBuffer);
    registerMouse(&mouseBuffer);
}

void keyPress(uint8_t scancode, bool pressed)
{
    if (pressed && scancode == SUPER_KEY)
    {
        dragging = 0;
        focus = 0;
        mouseFree = true;
    }
    else if (focus)
    {
        focus->events[focus->eventsHead].type = KEY_EVENT;
        focus->events[focus->eventsHead].keyEvent.scancode = scancode;
        focus->events[focus->eventsHead].keyEvent.pressed = pressed;
        focus->eventsHead++;
    }
}

void mouseMove(int16_t x, int16_t y)
{
    if (mouseFree)
    {
        if (!dragging)
        {
            int64_t newX = (int64_t)mouseX + x;
            int64_t newY = (int64_t)mouseY - y;
            if (newX < 0)
            {
                newX = 0;
            }
            else if (newX > display.width - 1)
            {
                newX = display.width - 1;
            }
            if (newY < 0)
            {
                newY = 0;
            }
            else if (newY > display.height - 1)
            {
                newY = display.height - 1;
            }
            mouseX = newX;
            mouseY = newY;
            if (focus && !focus->lockMouse && checkOverlap(focus->x + 10, focus->y + 47, focus->bufferWidth, focus->bufferHeight))
            {
                focus->events[focus->eventsHead].type = MOUSE_MOVE_EVENT;
                focus->events[focus->eventsHead].mouseMoveEvent.x = mouseX - (focus->x + 10);
                focus->events[focus->eventsHead].mouseMoveEvent.y = mouseY - (focus->y + 47);
                focus->eventsHead++;
            }
        }
        else
        {
            int64_t newX = (int64_t)dragging->x + x;
            int64_t newY = (int64_t)dragging->y - y;
            if (newX < 0)
            {
                newX = 0;
            }
            else if (newX > display.width - dragging->width)
            {
                newX = display.width - dragging->width;
            }
            if (newY < 0)
            {
                newY = 0;
            }
            else if (newY > display.height - dragging->height - 32)
            {
                newY = display.height - dragging->height - 32;
            }
            dragging->x = newX;
            dragging->y = newY;
            mouseX = dragging->x + draggingX;
            mouseY = dragging->y + draggingY;
        }
    }
    else
    {
        focus->events[focus->eventsHead].type = MOUSE_MOVE_EVENT;
        focus->events[focus->eventsHead].mouseMoveEvent.x = x;
        focus->events[focus->eventsHead].mouseMoveEvent.y = y;
        focus->eventsHead++;
    }
}

void desktopClick(bool left, bool pressed)
{
    bool nothing = true;
    WindowElement* element = windows->prev;
    while (true)
    {
        if (element->window && !element->window->minimised)
        {
            if (checkOverlap(element->window->x, element->window->y, element->window->width, element->window->height))
            {
                nothing = false;
                if (checkOverlap(element->window->x, element->window->y, element->window->width, 47))
                {
                    if (left)
                    {
                        if (checkOverlap(element->window->x + element->window->width - 42, element->window->y + 10, 32, 32))
                        {
                            element->window->events[element->window->eventsHead].type = CLOSE_EVENT;
                            element->window->eventsHead++;
                        }
                        else if (checkOverlap(element->window->x + element->window->width - 79, element->window->y + 10, 32, 32))
                        {
                            element->window->minimised = true;
                        }
                        else
                        {
                            if (focus != element->window)
                            {
                                focus = element->window;
                                ((WindowElement*)element->prev)->next = element->next;
                                ((WindowElement*)element->next)->prev = element->prev;
                                ((WindowElement*)windows->prev)->next = element;
                                element->next = windows;
                                element->prev = windows->prev;
                                windows->prev = element;
                            }
                            draggingX = (int64_t)mouseX - focus->x;
                            draggingY = (int64_t)mouseY - focus->y;
                            dragging = focus;
                        }
                    }
                }
                else if (checkOverlap(element->window->x + 10, element->window->y + 47, element->window->bufferWidth, element->window->bufferWidth))
                {
                    if (focus != element->window)
                    {
                        focus = element->window;
                        ((WindowElement*)element->prev)->next = element->next;
                        ((WindowElement*)element->next)->prev = element->prev;
                        ((WindowElement*)windows->prev)->next = element;
                        element->next = windows;
                        element->prev = windows->prev;
                        windows->prev = element;
                    }
                    if (focus->lockMouse && mouseFree)
                    {
                        mouseFree = false;
                    }
                    else
                    {
                        focus->events[focus->eventsHead].type = MOUSE_CLICK_EVENT;
                        focus->events[focus->eventsHead].mouseClickEvent.left = left;
                        focus->events[focus->eventsHead].mouseClickEvent.pressed = pressed;
                        focus->eventsHead++;
                    }
                }
                break;
            }
        }
        element = element->prev;
        if (element == windows->prev)
        {
            break;
        }
    }
    if (nothing)
    {
        focus = 0;
    }
}

void taskbarClick(bool left)
{
    if (checkOverlap(4, display.height - 28, 24, 24))
    {
        if (left)
        {
            exitDesktop();
        }
    }
    else
    {
        bool check = true;
        if (left)
        {
            for (uint64_t i = 0; i < taskbarIcons; i++)
            {
                if (checkOverlap(36 + i * 32, display.height - 28, 24, 24))
                {
                    check = false;
                    execute(taskbar[i].program);
                    break;
                }
            }
        }
        if (check)
        {
            uint64_t i = taskbarIcons + 1;
            WindowElement* element = taskbarWindows;
            while (true)
            {
                if (element->window)
                {
                    if (checkOverlap(5 + i * 32, display.height - 28, 24, 24))
                    {
                        if (left)
                        {
                            if (element->window->minimised)
                            {
                                element->window->minimised = false;
                                focus = element->window;
                                WindowElement* windowsElement = windows;
                                while (windowsElement->window != element->window)
                                {
                                    windowsElement = windowsElement->next;
                                }
                                ((WindowElement*)windowsElement->prev)->next = windowsElement->next;
                                ((WindowElement*)windowsElement->next)->prev = windowsElement->prev;
                                ((WindowElement*)windows->prev)->next = windowsElement;
                                windowsElement->next = windows;
                                windowsElement->prev = windows->prev;
                                windows->prev = windowsElement;
                            }
                            else if (focus == element->window)
                            {
                                element->window->minimised = true;
                            }
                            else
                            {
                                focus = element->window;
                                WindowElement* windowsElement = windows;
                                while (windowsElement->window != element->window)
                                {
                                    windowsElement = windowsElement->next;
                                }
                                ((WindowElement*)windowsElement->prev)->next = windowsElement->next;
                                ((WindowElement*)windowsElement->next)->prev = windowsElement->prev;
                                ((WindowElement*)windows->prev)->next = windowsElement;
                                windowsElement->next = windows;
                                windowsElement->prev = windows->prev;
                                windows->prev = windowsElement;
                            }
                        }
                        else
                        {
                            element->window->events[element->window->eventsHead].type = CLOSE_EVENT;
                            element->window->eventsHead++;
                        }
                    }
                    i++;
                }
                element = element->next;
                if (element == taskbarWindows)
                {
                    break;
                }
            }
        }
    }
}

void mouseClick(bool left, bool pressed)
{
    if (mouseFree)
    {
        if (pressed)
        {
            if (mouseY < display.height - 32)
            {
                desktopClick(left, pressed);
            }
            else
            {
                taskbarClick(left);
            }
        }
        else
        {
            if (left)
            {
                dragging = 0;
            }
        }
    }
    else
    {
        focus->events[focus->eventsHead].type = MOUSE_CLICK_EVENT;
        focus->events[focus->eventsHead].mouseClickEvent.left = left;
        focus->events[focus->eventsHead].mouseClickEvent.pressed = pressed;
        focus->eventsHead++;
    }
}

void processInputs()
{
    while (keyboardBuffer.tail != keyboardBuffer.head)
    {
        keyPress(keyboardBuffer.buffer[keyboardBuffer.tail].scancode, keyboardBuffer.buffer[keyboardBuffer.tail].pressed);
        keyboardBuffer.tail++;
    }
    while (mouseBuffer.tail != mouseBuffer.head)
    {
        if (mouseBuffer.buffer[mouseBuffer.tail].x != 0 || mouseBuffer.buffer[mouseBuffer.tail].y != 0)
        {
            mouseMove(mouseBuffer.buffer[mouseBuffer.tail].x, mouseBuffer.buffer[mouseBuffer.tail].y);
        }
        if (mouseBuffer.buffer[mouseBuffer.tail].left)
        {
            if (!lastLeft)
            {
                mouseClick(true, true);
                lastLeft = true;
            }
        }
        else
        {
            if (lastLeft)
            {
                mouseClick(true, false);
                lastLeft = false;
            }
        }
        if (mouseBuffer.buffer[mouseBuffer.tail].right)
        {
            if (!lastRight)
            {
                mouseClick(false, true);
                lastRight = true;
            }
        }
        else
        {
            if (lastRight)
            {
                mouseClick(false, false);
                lastRight = false;
            }
        }
        mouseBuffer.tail++;
    }
}

void cleanupInputs()
{
    unregisterKeyboard(&keyboardBuffer);
    unregisterMouse(&mouseBuffer);
}
