#ifndef _VM_H
#define _VM_H

struct page_table {
    u32_t entry_l;
    u32_t entry_h;
} __attribute__((packed));

// 4096 * 32 bit * 2MB = 256GB
#define PT_BITMAPSIZE 4096

struct pt_bitmap_t {
    u32_t map[2048]; //can map 2TB
};

static struct pt_bitmap_t pt_bitmap;

#define PML4_SHIFT 39
#define PDPT_SHIFT 30
#define PD_SHIFT   21
#define PT_SHIFT   12

#define PTE_PRESENT   1 << 0
#define PTE_WRITEABLE 1 << 1
#define PTE_USER      1 << 2
#define PTE_ACCESSED  1 << 5
#define PTE_DIRT      1 << 6

#define DEFAULT_PT_FLAGS PTE_PRESENT | PTE_WRITEABLE

#define PT_START_ADDR 0x70000

#define TABLE_ADDR 0xffffff00

typedef enum page_table_type { PT = 1, PD, PDPT, PML4} pt_t;

#define PT_OFFSET 0x1000 /* 4kb */

extern u8_t volatile *_pt_base_address; /* u8_t to have increments of 1 */

struct lat *linear_address_trans;

u64_t *flush_page_table(u64_t *pt);
void pt_bitmap_set(u32_t bit, bool b);
bool pt_bitmap_get(u32_t bit);

#endif _VM_H
