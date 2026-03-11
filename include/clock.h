#pragma once

#include <definitions.h>

extern uint8_t hour;
extern uint8_t minute;
extern uint8_t second;
extern bool half;

void initClock();

void cleanupClock();
