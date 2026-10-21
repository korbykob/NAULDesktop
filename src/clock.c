#include <clock.h>
#include <rtc.h>
#include <hpet.h>
#include <scheduler.h>
#include <cpu.h>

uint64_t timeThreadId = 0;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
bool half = false;
bool timeRunning = true;

void updateTime()
{
    getTime(&hour, &minute, &second);
    second = 60 - second;
    half = false;
    if (hour >= 12)
    {
        half = true;
        if (hour != 12)
        {
            hour -= 12;
        }
    }
    else if (hour == 0)
    {
        hour = 12;
    }
}

void timeThread()
{
    while (timeRunning)
    {
        waitFemtoseconds(FEMTOSECONDS_PER_SECOND * second);
        updateTime();
    }
}

void initClock()
{
    updateTime();
    timeThreadId = createThread(timeThread);
}

void cleanupClock()
{
    timeRunning = false;
    cancelWait(timeThreadId);
    waitForThread(timeThreadId);
}
