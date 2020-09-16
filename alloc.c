#include "utils.h"
#include "alloc.h"

struct bump_alloc *b_alloc_init() {
    struct bump_alloc *default_kalloc;
    
    default_kalloc->nextp = (u64_t*) K_HEAP_START;
    default_kalloc->alloc_count = 0;

    return default_kalloc; 
}

void* b_alloc(struct bump_alloc *ba, u64_t size) {
    if ((*ba->nextp + size) >= K_HEAP_END - 1)
        return NULL;    
    
    ba->alloc_count++;
    *ba->nextp += size;
    
    return (*ba->nextp - size);    
}

void kmalloc_init(void) {
    kheap = b_alloc_init();    
}

void* kmalloc(u64_t size) {
    return b_alloc(kheap, size);
}