#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef __CUSTOM_TYPES
#define __CUSTOM_TYPES

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

#endif

typedef struct Display Display;

typedef enum {
    DISPLAY_TYPE_CRT,
    DISPLAY_TYPE_LCD
} DisplayType;

typedef struct {
    word width, height;
    dword area;

    word hblank, vblank;
    word full_width, full_height;
    dword full_area;
} SignalSize;

Display *display_new(const char *title, dword width, dword height, DisplayType type);
void display_destroy(Display **display);

bool display_should_close(Display *display);

bool display_set_signal_size(Display *display, word width, word height, word hblank, word vblank);

void display_draw_pixel(Display *display, dword color);
void display_draw_pixels(Display *display, dword color, size_t count);

void display_update(Display *display);

#endif