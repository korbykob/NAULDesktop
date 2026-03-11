#pragma once

#include <definitions.h>
#include <windows.h>

#define CLOSE_EVENT 0
#define KEY_EVENT 1
#define MOUSE_MOVE_EVENT 2
#define MOUSE_CLICK_EVENT 3

extern uint32_t mouseX;
extern uint32_t mouseY;
extern Window* focus;
extern Window* dragging;
extern bool mouseFree;

void initInputs();

void processInputs();

void cleanupInputs();
