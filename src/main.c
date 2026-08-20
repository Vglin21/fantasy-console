#include <dot-5/display.h>
#include <dot-5/cpu.h>
#include <stdio.h>

byte rom[] = {
    LDA_Z, 0x0,
    AND_I, 0x2,
    BEQ, 0x5,
    LDA_Z, 0x1,
    INC,
    STA, 0x1,

    LDA_Z, 0x0,
    AND_I, 0x4,
    BEQ, 0x5,
    LDA_Z, 0x1,
    DEC,
    STA, 0x1,

    LDA_Z, 0x0,
    AND_I, 0x8,
    BEQ, 0x6,
    LDA_Z, 0x1,
    ADD_I, 0x10,
    STA, 0x1,

    LDA_Z, 0x0,
    AND_I, 0x10,
    BEQ, 0x6,
    LDA_Z, 0x1,
    SUB_I, 0x10,
    STA, 0x1,

    LDA_I, 0x0,
    STA, 0x0,
    LDA_Z, 0x0,
    BEQ, 0xfc,
    JMP, 0x8
};

int main() {
    display_turn_on("DOT-5", 640, 640, DISPLAY_TYPE_LCD);
    display_set_fps(11.97);

    display_set_signal_size(16, 16, 0, 0);

    mem_load_rom(rom, sizeof(rom));

    word beam = 0;
    while (!display_should_close()) {
        while (display_is_frame_active()) {
            byte cycles = cpu_step();

            while (cycles--) {
                if (beam < 256) {
                    bool dot = false;
                    for (byte c = 1; c < 6; ++c) if (beam == mem_read(c)) {
                        dot = true;
                        break;
                    }
                    display_draw_pixel(dot ? 0x070707ff : 0xe7e7e7ff);

                    ++beam;
                } else if (beam == 256) {
                    mem_write(0, 1);
                    if (display_is_key_pressed(DISPK_RIGHT)) mem_write(0, mem_read(0) | 2);
                    if (display_is_key_pressed(DISPK_LEFT))  mem_write(0, mem_read(0) | 4);
                    if (display_is_key_pressed(DISPK_DOWN))  mem_write(0, mem_read(0) | 8);
                    if (display_is_key_pressed(DISPK_UP))    mem_write(0, mem_read(0) | 16);

                    ++beam;
                } else if ((++beam) == 320) beam = 0;
            }

            printf(
                "-------------------------\n"
                "|00|01|02|03|04|05|06|07|\n"
                "-------------------------\n"
                "|%.2x|%.2x|%.2x|%.2x|%.2x|%.2x|%.2x|%.2x|\n"
                "-------------------------\n",
                mem_read(0),
                mem_read(1),
                mem_read(2),
                mem_read(3),
                mem_read(4),
                mem_read(5),
                mem_read(6),
                mem_read(7)
            );
        }

        display_update();
    }

    display_turn_off();

    FILE *file = fopen("output.rom", "w");

    fwrite(rom, 1, sizeof(rom), file);

    fclose(file);

    return 0;
}