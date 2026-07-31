#include <fancon/display.h>

int main() {
    Display *display = display_new("Fantasy Console", 640, 640, DISPLAY_TYPE_CRT);

    display_set_signal_size(display, 12, 12, 5, 4);

    dword color = 0xffff;
    while (!display_should_close(display)) {
        display_draw_pixel(display, (color += 0xff0000));

        display_update(display);
    }

    display_destroy(&display);

    return 0;
}