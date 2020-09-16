#include "utils.h"
#include "smap.h"

char *e820_type(u8_t type) {
    switch (type)
    {
    case 1:
        return "usable";
    case 2:
        return "reserved";
    default:
        return 0; //strdup ?
    }
}

u64_t e820_usable_memory() {
    static struct e820_SMAP *map[SMAP_MAX_ENTRIES];
    
    u64_t size = 0;
    e820_mem_map(map, USABLE_MEM);
    
    for (u8_t i=0; map[i]->type !=0; i++)
        size +=map[i]->lenght;        
    
    // return in MB
    return size / (1024*1024);
}

void e820_SMAP_print() {
    static struct e820_SMAP *map[SMAP_MAX_ENTRIES];
    
    kprintf("yolt: e820 smap table:\n");
    e820_mem_map(map, ANY_MEM);

    for (u8_t i=0; map[i]->type !=0; i++)
            kprintf("  region [ 0x%16x - 0x%16x ] %s\n", map[i]->base_addr, map[i]->lenght, e820_type(map[i]->type));             

    kprintf("yolt: %d MB usable memory.\n", e820_usable_memory());
}

void e820_mem_map(struct e820_SMAP **map, u8_t type) {
    struct e820_SMAP *addrMap = (struct e820_SMAP*) &_bios_mem_map;

    u8_t r = 0;
    for (u8_t i=0; i < _bios_mem_count; i++) {                      
        if ((int) addrMap->type == type || type == ANY_MEM)    
            map[r++] = addrMap;
        addrMap++;    
    }    
}