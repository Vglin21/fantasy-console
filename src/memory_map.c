#include <dot-5/memory_map.h>

static byte memory[0x100] = {0};

void mem_load_rom(byte *rom, byte size) {
    byte s = size > 248 ? 248 : size;
    for (byte c = 0; c < s; ++c) memory[c+8] = rom[c];
}

byte mem_read(byte address) { return memory[address]; }
void mem_write(byte address, byte value) { if (address < 8) memory[address] = value; }