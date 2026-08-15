#include <dot-5/memory_map.h>
#include <stdlib.h>
#include <string.h>

#define MAX_AR_AMOUNT 8

struct AddressRange {
    byte *mem;
    dword start;
    dword end;
    dword size;
    bool read_only;
};

static AddressRange memory[MAX_AR_AMOUNT] = {0};

AddressRange *get_ar(dword address) {
    for (dword i = 0; i < MAX_AR_AMOUNT; ++i) {
        if (memory[i].start <= address && address < memory[i].end) {
            return &memory[i];
        }
    }
    return NULL;
}

AddressRange *mem_new_address_range(dword size, dword pos, bool read_only) {
    for (dword i = 0; i < MAX_AR_AMOUNT; ++i) {
        if (memory[i].mem == NULL) {
            AddressRange *ar = &memory[i];

            if (!(ar->mem = (byte*)malloc(size))) return NULL;

            ar->start = pos;
            ar->end = pos + size;
            ar->size = size;
            ar->read_only = read_only;

            return ar;
        }
    }
    return NULL;
}
void mem_free_address_range(AddressRange *ar) {
    for (dword i = 0; i < MAX_AR_AMOUNT; ++i) {
        if (memory[i].mem == ar->mem) {
            free(ar->mem);
            ar->mem = NULL;
            ar->start = 0;
            ar->end = 0;
            ar->size = 0;
            ar->read_only = false;
            break;
        }
    }
}
void mem_free_all() {
    for (dword i = 0; i < MAX_AR_AMOUNT; ++i) {
        if (memory[i].mem != NULL) {
            free(memory[i].mem);
            memory[i].mem = NULL;
            memory[i].start = 0;
            memory[i].end = 0;
            memory[i].size = 0;
            memory[i].read_only = false;
        }
    }
}

void mem_load(AddressRange *ar, byte *src, dword size) {
    if (ar && ar->mem && src) {
        memcpy(ar->mem, src, size > ar->size ? ar->size : size);
    }
}

byte mem_read(dword address) {
    AddressRange *ar = get_ar(address);
    return ar ? ar->mem[address - ar->start] : 0;
}
void mem_write(dword address, byte value) {
    AddressRange *ar = get_ar(address);
    if (ar && !ar->read_only) ar->mem[address - ar->start] = value;
}

byte mem_read_from(AddressRange *ar, dword address) {
    if (ar && address < ar->size)
        return ar->mem[address];
}
void mem_write_to(AddressRange *ar, dword address, byte value) {
    if (ar && !ar->read_only && address < ar->size)
        ar->mem[address] = value;
}