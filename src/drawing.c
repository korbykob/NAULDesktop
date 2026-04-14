#include <drawing.h>
#include <taskbar.h>
#include <inputs.h>
#include <clock.h>
#include <desktop.h>
#include <display.h>
#include <filesystem.h>
#include <allocator.h>
#include <str.h>
#include <mem.h>
#include <bmp.h>
#include <psf.h>

#define MOUSE_SIZE 16
#define WHITE 0xFFFFFF
#define GREY 0x808080
#define BLACK 0x000000
#define RED 0xFF0000

Display display;
const uint8_t mouseIcon[] = {
    0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,
    0,1,0,0,0,2,2,2,2,2,2,2,2,2,2,2,
    0,1,1,1,1,0,0,0,2,2,2,2,2,2,2,2,
    0,1,1,1,1,1,1,0,0,2,2,2,2,2,2,2,
    0,0,1,1,1,1,1,1,1,0,0,0,2,2,2,2,
    0,0,1,1,1,1,1,1,1,1,0,0,2,2,2,2,
    2,0,1,1,1,1,1,1,1,1,0,2,2,2,2,2,
    2,0,1,1,1,1,1,1,1,0,2,2,2,2,2,2,
    2,0,0,1,1,1,1,0,0,0,0,2,2,2,2,2,
    2,2,0,1,0,0,0,0,1,1,0,0,2,2,2,2,
    2,2,0,0,0,2,0,0,1,1,1,0,0,0,0,2,
    2,2,2,0,2,2,2,0,0,0,1,1,1,1,0,2,
    2,2,2,2,2,2,2,2,2,2,0,0,0,1,0,2,
    2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};
const uint8_t exitIcon[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,1,1,1,0,1,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,
    1,1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,
};
uint32_t* backBuffer = 0;
PsfFile* font = 0;
uint32_t* wallpaper = 0;

void loadWallpaper()
{
    wallpaper = allocate(display.pitch * display.height * sizeof(uint32_t));
    uint32_t* buffer = wallpaper;
    BmpHeader* wallpaperFile = (BmpHeader*)getFile("/programs/desktop/wallpaper.bmp", 0);
    for (uint32_t y = 0; y < display.height; y++)
    {
        for (uint32_t x = 0; x < display.width; x++)
        {
            *buffer++ = *(uint32_t*)((uint8_t*)wallpaperFile + wallpaperFile->offset + (((wallpaperFile->height - (uint32_t)(wallpaperFile->height * (y / (double)display.height)) - 1) * wallpaperFile->width + (uint32_t)(wallpaperFile->width * (x / (double)display.width))) * 3)) & 0x00FFFFFF;
        }
        buffer += display.pitch - display.width;
    }
}

void initDrawing()
{
    obtainDisplay(&display);
    loadWallpaper();
    mouseX = display.width / 2;
    mouseY = display.height / 2;
    font = (PsfFile*)getFile("/naul/font.psf", 0);
    backBuffer = allocate(display.pitch * display.height * sizeof(uint32_t));
    setMemory32(backBuffer, 0, display.pitch * display.height);
}

void drawBuffer(uint32_t* buffer, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
{
    uint32_t* address = backBuffer + y * display.pitch + x;
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
           *address++ = *buffer++;
        }
        address += display.pitch - width;
    }
}

void drawColour(uint32_t colour, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
{
    uint32_t* address = backBuffer + y * display.pitch + x;
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
           *address++ = colour;
        }
        address += display.pitch - width;
    }
}

void drawString(const char* string, uint32_t colour, uint32_t x, uint32_t y)
{
    while (*string)
    {
        uint32_t* address = backBuffer + y * display.pitch + x;
        uint8_t* glyph = font->data + font->glyphSize * *string;
        for (uint32_t y = 0; y < font->height; y++)
        {
            uint32_t width = font->width;
            while (width)
            {
                uint32_t amount = min(8, width);
                for (uint8_t x = 0; x < amount; x++)
                {
                    if (*glyph & (0b10000000 >> x))
                    {
                        *address = colour;
                    }
                    address++;
                }
                glyph++;
                width -= amount;
            }
            address += display.pitch - font->width;
        }
        x += font->width + 1;
        string++;
    }
}

void drawWindows()
{
    WindowElement* element = windows;
    while (true)
    {
        if (element->window && !element->window->minimised)
        {
            drawColour(focus == element->window ? WHITE : BLACK, element->window->width, element->window->height, element->window->x, element->window->y);
            drawColour(GREY, element->window->width - 10, element->window->height - 10, element->window->x + 5, element->window->y + 5);
            drawBuffer(element->window->icon, 24, 24, element->window->x + 14, element->window->y + 14);
            drawString(element->window->title, BLACK, (element->window->x + element->window->width / 2) - ((stringLength(element->window->title) * 17) / 2), element->window->y + 10);
            drawColour(RED, 32, 32, element->window->x + element->window->width - 42, element->window->y + 10);
            drawString("X", BLACK, element->window->x + element->window->width - 34, element->window->y + 10);
            drawColour(BLACK, 32, 32, element->window->x + element->window->width - 79, element->window->y + 10);
            drawString("-", WHITE, element->window->x + element->window->width - 71, element->window->y + 10);
            drawBuffer(element->window->buffer, element->window->bufferWidth, element->window->bufferHeight, element->window->x + 10, element->window->y + 47);
        }
        element = element->next;
        if (element == windows)
        {
            break;
        }
    }
}

void drawTaskbar()
{
    drawColour(GREY, display.width, 32, 0, display.height - 32);
    uint32_t* address = backBuffer + (display.height - 28) * display.pitch + 4;
    const uint8_t* buffer = exitIcon;
    for (uint8_t y = 0; y < 24; y++)
    {
        for (uint8_t x = 0; x < 24; x++)
        {
            *address++ = *buffer++ ? RED : WHITE;
        }
        address += display.pitch - 24;
    }
    for (uint64_t i = 0; i < taskbarIcons; i++)
    {
        drawBuffer(taskbar[i].icon, 24, 24, 36 + i * 32, display.height - 28);
    }
    drawColour(BLACK, 2, 24, taskbarIcons * 32 + 31, display.height - 28);
    uint64_t i = taskbarIcons + 1;
    WindowElement* element = taskbarWindows;
    while (true)
    {
        if (element->window)
        {
            if (!element->window->minimised)
            {
                drawColour(BLACK, 24, 2, 5 + i * 32, display.height - 2);
                if (focus == element->window)
                {
                    drawColour(BLACK, 28, 28, 3 + i * 32, display.height - 30);
                }
            }
            drawBuffer(element->window->icon, 24, 24, 5 + i * 32, display.height - 28);
            i++;
        }
        element = element->next;
        if (element == taskbarWindows)
        {
            break;
        }
    }
    drawString(half ? "PM" : "AM", WHITE, display.width - 37, display.height - 32);
    char characters[3];
    toString(characters, minute);
    drawString(characters, WHITE, display.width - ((minute < 10) ? 69 : 85), display.height - 32);
    if (minute < 10)
    {
        drawString("0", WHITE, display.width - 85, display.height - 32);
    }
    drawString(":", WHITE, display.width - 101, display.height - 32);
    toString(characters, hour);
    drawString(characters, WHITE, display.width - ((hour < 10) ? 117 : 133), display.height - 32);
    if (hour < 10)
    {
        drawString("0", WHITE, display.width - 133, display.height - 32);
    }
}

void drawMouse()
{
    if (mouseFree)
    {
        uint32_t* address = backBuffer + mouseY * display.pitch + mouseX;
        const uint8_t* mouseBuffer = mouseIcon;
        for (uint8_t y = 0; y < MOUSE_SIZE; y++)
        {
            for (uint8_t x = 0; x < MOUSE_SIZE; x++)
            {
                if (*mouseBuffer != 2 && mouseX + x < display.width && mouseY + y < display.height)
                {
                    *address = *mouseBuffer ? WHITE : BLACK;
                }
                mouseBuffer++;
                address++;
            }
            address += display.pitch - MOUSE_SIZE;
        }
    }
}

void draw()
{
    copyMemory32(wallpaper, backBuffer, display.pitch * display.height);
    drawWindows();
    drawTaskbar();
    drawMouse();
    copyMemory32(backBuffer, display.buffer, display.pitch * display.height);
}

void cleanupDrawing()
{
    releaseDisplay();
    unallocate(wallpaper);
    unallocate(backBuffer);
}
