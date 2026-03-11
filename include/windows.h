#pragma once

#include <definitions.h>

#define IPC_NAME "desktop"

enum DesktopCalls
{
    CREATE_WINDOW,
    DESTROY_WINDOW
};
typedef union
{
    uint8_t type;
    struct
    {
        uint8_t type;
    } closeEvent;
    struct
    {
        uint8_t type;
        uint8_t scancode;
        bool pressed;
    } keyEvent;
    struct
    {
        uint8_t type;
        int32_t x;
        int32_t y;
    } mouseMoveEvent;
    struct
    {
        uint8_t type;
        bool left;
        bool pressed;
    } mouseClickEvent;
} WindowEvent;
typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool minimised;
    bool lockMouse;
    uint32_t bufferWidth;
    uint32_t bufferHeight;
    uint32_t* buffer;
    uint8_t eventsHead;
    uint8_t eventsTail;
    WindowEvent events[256];
    char title[64];
    uint32_t icon[24 * 24];
} Window;
typedef struct
{
    void* next;
    void* prev;
    Window* window;
} WindowElement;

extern WindowElement* windows;
extern WindowElement* taskbarWindows;

void initWindows();

void cleanupWindows();
