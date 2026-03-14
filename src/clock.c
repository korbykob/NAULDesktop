#include <clock.h>
#include <rtc.h>
#include <hpet.h>
#include <scheduler.h>

uint64_t timeThreadId = 0;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
bool half = false;

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
    uint64_t last = getFemtoseconds();
    while (true)
    {
        uint64_t femtoseconds = getFemtoseconds();
        if (femtoseconds - last >= FEMTOSECONDS_PER_SECOND * second)
        {
            last = femtoseconds;
            updateTime();
        }
        yieldThread();
    }
}

void initClock()
{
    timeThreadId = createThread(timeThread);
    updateTime();
}

void cleanupClock()
{
    destroyThread(timeThreadId);
}
