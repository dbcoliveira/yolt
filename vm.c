#include "utils.h"
#include "vm.h"

static u32_t bitmap_bits;

void setup_paging(void) { 
    bitmap_bits = sizeof(pt_bitmap.map[0]) * 8; // 8 bits
    
    for (int i = 0 ; i < PT_BITMAPSIZE; i++)
        pt_bitmap.map[i] = 0;
}

bool pt_bitmap_get(u32_t bit) {    
    return check_bit(pt_bitmap.map[bit / bitmap_bits] , bit % bitmap_bits);
}

void pt_bitmap_set(u32_t bit, bool b) {    
    pt_bitmap.map[bit / bitmap_bits] = (pt_bitmap.map[bit / bitmap_bits] & ~(1<<bit)) | ((b << bit) & (1<<bit)); 
}

u64_t pt_next_avail(void) {    
    u64_t r = bitmap_bits;
    for (int i = 0; i < PT_BITMAPSIZE; i++) 
       if ((r = _ffz(pt_bitmap.map[i])) < bitmap_bits) 
           return (bitmap_bits * i) + r;

    return -1;      
}

u32_t pt_counter = 0;

u64_t *new_page_table(void) 
{
    u8_t* pt_next = (u8_t*) PT_START_ADDR;
    u8_t page = pt_next_avail();
    pt_next += PT_OFFSET * page;
    
    kprintf("\npage_addr = 0x%x, page = %d, index = ",pt_next, page);

    for (int e = 0 ; e < 16; e++)
        kprintf("%s",itoa(pt_bitmap_get(e),2));
    
    for(int i = 0 ; i < 512; i++)
        ((struct page_table *) (pt_next))[i].entry_l |= PTE_WRITEABLE; 
    
    pt_bitmap_set(page, true);
    return pt_next;
}

u64_t *flush_pt(u64_t *pt) {

    for(int i = 0 ; i < 512; i++)
        ((struct page_table *) (pt))[i].entry_l = 0; 
    
    u64_t page = ((u64_t)pt - PT_START_ADDR) / PT_OFFSET;

    pt_bitmap_set(page, false);
    pt_counter--;    

    return pt;
}

static bool pt_entry_valid (u8_t e) {
    return e & PTE_PRESENT;
}

u32_t get_pte_index(pt_t pt_type, u64_t vrt_addr) {
/*
    u32_t m4 = (a >> 39) & 0x1ff;
    u32_t m3 = (a >> 30) & 0x1ff;
    u32_t m2 = (a >> 21) & 0x1ff;
    u32_t m1 = (a >> 12) & 0x1ff;
    u32_t offset = a & 0xfff;    
*/
    switch (pt_type) {
        case PML4:
            return (vrt_addr >> PML4_SHIFT) & 0x1ff;
        case PDPT:
            return (vrt_addr >> PDPT_SHIFT) & 0x1ff;
        case PD:
            return (vrt_addr >> PD_SHIFT) & 0x1ff;
        case PT: 
            return (vrt_addr >> PT_SHIFT) & 0x1ff;
        default:
            return -1;
    }
}

void map_addr(u32_t phy_addr) {    

    if (phy_addr <= 0)
        return;    

    //kprintf("\n 0x%x, index= ", phy_addr);

    struct page_table *pml4 = (u32_t volatile) _pt_base_address;

    struct page_table *pdpt = (pml4->entry_l & TABLE_ADDR);
    u16_t index = get_pte_index(PML4,phy_addr);
    if (!pt_entry_valid(pml4[index].entry_l)) {
        struct page_table *new_pt = new_page_table();
        pml4[index].entry_l = ((u64_t)new_pt) | DEFAULT_PT_FLAGS;
        pdpt = new_pt;        
    }

    //kprintf("PML4:0x%x", index);

    struct page_table *pde = (pdpt->entry_l & TABLE_ADDR);
    index = get_pte_index(PDPT,phy_addr);
    if (!pt_entry_valid(pdpt[index].entry_l)) {
        struct page_table *new_pt = new_page_table();
        pdpt[index].entry_l = ((u64_t)new_pt) | DEFAULT_PT_FLAGS;
        pde = new_pt;        
    } 

    //kprintf(",PTPD:0x%x", index);

    struct page_table *pte = (pde->entry_l & TABLE_ADDR);
    index = get_pte_index(PD,phy_addr);
    if (!pt_entry_valid(pde[index].entry_l)) {
        struct page_table *new_pt = new_page_table(); 
        pde[index].entry_l = ((u64_t)new_pt) | DEFAULT_PT_FLAGS;
        pte = new_pt;        
    }

    //kprintf(",PD:0x%x,PT:0x%x");//, index, get_pte_index(PT,phy_addr));

    index = get_pte_index(PT,phy_addr);
    if (!pt_entry_valid(pte->entry_l))
        pte[index].entry_l = phy_addr & TABLE_ADDR | DEFAULT_PT_FLAGS;
    //else kprint("\n *** page already exists. ***\n");    
}
