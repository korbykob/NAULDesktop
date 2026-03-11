#include <taskbar.h>
#include <filesystem.h>
#include <allocator.h>
#include <str.h>
#include <bmp.h>

uint64_t taskbarIcons = 0;
TaskbarIcon* taskbar = 0;

void initTaskbar()
{
    uint64_t count = 0;
    const char** files = getFiles("/programs", &count);
    for (uint64_t i = 0; i < count; i++)
    {
        char* name = allocate(stringLength(files[i]) + 13);
        copyString(files[i], name);
        addString("/desktop.bmp", name);
        if (checkFile(name))
        {
            taskbarIcons++;
        }
        unallocate(name);
    }
    taskbar = allocate(taskbarIcons * sizeof(TaskbarIcon));
    taskbarIcons = 0;
    for (uint64_t i = 0; i < count; i++)
    {
        uint64_t length = stringLength(files[i]);
        char* name = allocate(length + 13);
        copyString(files[i], name);
        addString("/desktop.bmp", name);
        if (checkFile(name))
        {
            copyString(files[i], taskbar[taskbarIcons].program);
            taskbar[taskbarIcons].program[length] = '/';
            taskbar[taskbarIcons].program[length + 1] = '\0';
            char* programName = (char*)files[i] + length;
            while (*programName != '/')
            {
                programName--;
            }
            programName++;
            addString(programName, taskbar[taskbarIcons].program);
            addString(".nxe", taskbar[taskbarIcons].program);
            readBmp((BmpHeader*)getFile(name, 0), taskbar[taskbarIcons].icon);
            taskbarIcons++;
        }
        unallocate(name);
    }
    unallocate(files);
}

void cleanupTaskbar()
{
    unallocate(taskbar);
}
