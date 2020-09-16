#ifndef _SMAP_H
#define _SMAP_H

#define SMAP_MAX_ENTRIES 16

enum BIOS_SMAP_MEM_TYPE { ANY_MEM = 0, USABLE_MEM = 1, RESERVED_MEM = 2};

struct e820_SMAP {
    u64_t base_addr;    
    u64_t lenght;
    u64_t type;
}  __attribute__((packed));

extern u32_t _bios_mem_count;   
extern u32_t *_bios_mem_map;

#endif _SMAP_H