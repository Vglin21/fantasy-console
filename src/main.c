#include <fancon/display.h>

int main() {
    display_turn_on("DOT-8", 640, 640, DISPLAY_TYPE_LCD);

    display_set_signal_size(16, 16, 0, 0);

    byte pos = 0x77;
    byte beam = 0;
    while (!display_should_close()) {
        while (display_is_frame_active()) {
            if (display_is_key_pressed(DISPK_UP))    pos -= 0x10;
            if (display_is_key_pressed(DISPK_LEFT))  pos--;
            if (display_is_key_pressed(DISPK_DOWN))  pos += 0x10;
            if (display_is_key_pressed(DISPK_RIGHT)) pos++;
    
            while (1) {
                display_draw_pixel(beam == pos ? 0xffffffff : 0xff);
                if ((beam++) == 255) break;
            }
        }

        display_update();
    }

    display_turn_off();

    return 0;
}