#ifndef _ALLOC_H
#define _ALLOC_H

// 256MB
#define K_HEAP_START 0xd0000000
#define K_HEAP_END 0xe0000000

struct bump_alloc *kheap;

struct bump_alloc {
    u64_t *nextp;
    u64_t alloc_count;
};

#endif _ALLOC_H