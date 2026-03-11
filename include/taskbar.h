#pragma once

#include <windows.h>

typedef struct
{
    uint32_t icon[24 * 24];
    char program[256];
} TaskbarIcon;

extern uint64_t taskbarIcons;
extern TaskbarIcon* taskbar;

void initTaskbar();

void cleanupTaskbar();
