#include <dot-5/display.h>
#include <dot-5/memory_map.h>

int main() {
    display_turn_on("DOT-5", 640, 640, DISPLAY_TYPE_LCD);
    display_set_fps(12);

    display_set_signal_size(16, 16, 0, 0);

    AddressRange *ram = mem_new_address_range(8, 0, false);
    AddressRange *rom = mem_new_address_range(248, 8, true);

    mem_write(0x1, 0x97);
    mem_write(0x2, 0x86);
    mem_write(0x3, 0x88);
    mem_write(0x4, 0x66);
    mem_write(0x5, 0x68);
    byte beam = 0;
    while (!display_should_close()) {
        while (display_is_frame_active()) {
            if (display_is_key_pressed(DISPK_UP))    mem_write_to(ram, 0x1, mem_read_from(ram, 0x1) - 0x10);
            if (display_is_key_pressed(DISPK_LEFT))  mem_write_to(ram, 0x1, mem_read_from(ram, 0x1) - 0x1);
            if (display_is_key_pressed(DISPK_DOWN))  mem_write_to(ram, 0x1, mem_read_from(ram, 0x1) + 0x10);
            if (display_is_key_pressed(DISPK_RIGHT)) mem_write_to(ram, 0x1, mem_read_from(ram, 0x1) + 0x1);
    
            while (1) {
                bool dot = false;
                for (byte i = 1; i < 6; ++i) if (beam == mem_read_from(ram, i)) {
                    dot = true;
                    break;
                }
                display_draw_pixel(dot ? 0x070707ff : 0xe7e7e7ff);
                if ((beam++) == 255) break;
            }
        }

        display_update();
    }

    mem_free_all();

    display_turn_off();

    return 0;
}