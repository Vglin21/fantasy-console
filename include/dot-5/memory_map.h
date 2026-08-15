#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef _CUSTOM_TYPES_
#define _CUSTOM_TYPES_

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

#endif

typedef struct AddressRange AddressRange;

extern AddressRange *mem_new_address_range(dword size, dword pos, bool read_only);
extern void mem_free_address_range(AddressRange *ar);
extern void mem_free_all();

extern void mem_load(AddressRange *ar, byte *src, dword size);

extern byte mem_read(dword address);
extern void mem_write(dword address, byte value);

extern byte mem_read_from(AddressRange *ar, dword address);
extern void mem_write_to(AddressRange *ar, dword address, byte value);

#endif