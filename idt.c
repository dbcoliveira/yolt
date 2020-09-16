#include "utils.h"
#include "idt.h"

#define GDT_CS_SELECTOR 0x8

#define INTERRUPT_GATE 0x8e

#define IDTable_sz 256

#define INIT_idt_entry(e, l, m, h, i) \
    struct IDT_entry_t *e; \
        e->GDT_CS_selector = GDT_CS_SELECTOR; \
        e->l_addr = (l); \
        e->m_addr = (m); \
        e->h_addr = (h); \
        e->type_attr = INTERRUPT_GATE; \
        e->reserved = 0; \    
    IDT[i] = *e; \

#define REGISTER_idt_entry(e, interrupt, IDTpos) \
    INIT_idt_entry(e, \
        (u16_t) interrupt & 0x0000ffff, \
        (u16_t) interrupt & 0xffff0000, \
        (u32_t) interrupt & 0xffffffff00000000, \
        IDTpos) ; \                                          

//struct IDT_entry_t *IDT;// = (void*) 0x800;
static struct IDT_entry_t IDT[IDTable_sz]; // move this to a known location (0x800)

void setupIDT()
{
    memset(IDT, 0, sizeof(struct IDT_entry_t) * IDTable_sz);

    REGISTER_idt_entry(irq0d_desc, _irq_entry_0, 0);
    REGISTER_idt_entry(irq1d_desc, _irq_entry_1, 1);
    REGISTER_idt_entry(irq2d_desc, _irq_entry_2, 2);
    REGISTER_idt_entry(irq3d_desc, _irq_entry_3, 3);
    REGISTER_idt_entry(irq4d_desc, _irq_entry_4, 4);    
    REGISTER_idt_entry(irq5d_desc, _irq_entry_5, 5);
    REGISTER_idt_entry(irq6d_desc, _irq_entry_6, 6);
    REGISTER_idt_entry(irq7d_desc, _irq_entry_7, 7);
    REGISTER_idt_entry(irq8d_desc, _irq_entry_8, 8);
    REGISTER_idt_entry(irq9d_desc, _irq_entry_9, 9);
    REGISTER_idt_entry(irq10d_desc, _irq_entry_10, 10);
    REGISTER_idt_entry(irq11d_desc, _irq_entry_11, 11);
    REGISTER_idt_entry(irq12d_desc, _irq_entry_12, 12);
    REGISTER_idt_entry(irq13d_desc, _irq_entry_13, 13);
    REGISTER_idt_entry(irq14d_desc, _irq_entry_14, 14);
    REGISTER_idt_entry(irq15d_desc, _irq_entry_15, 15);
    REGISTER_idt_entry(irq16d_desc, _irq_entry_16, 16);
    REGISTER_idt_entry(irq17d_desc, _irq_entry_17, 17);
    REGISTER_idt_entry(irq18d_desc, _irq_entry_18, 18);
    REGISTER_idt_entry(irq19d_desc, _irq_entry_19, 19);
    REGISTER_idt_entry(irq20d_desc, _irq_entry_20, 20);    
    REGISTER_idt_entry(irq21d_desc, _irq_entry_21, 21);
    REGISTER_idt_entry(irq22d_desc, _irq_entry_22, 22);
    REGISTER_idt_entry(irq23d_desc, _irq_entry_23, 23);
    REGISTER_idt_entry(irq24d_desc, _irq_entry_24, 24);
    REGISTER_idt_entry(irq25d_desc, _irq_entry_25, 25);
    REGISTER_idt_entry(irq26d_desc, _irq_entry_26, 26);
    REGISTER_idt_entry(irq27d_desc, _irq_entry_27, 27);
    REGISTER_idt_entry(irq28d_desc, _irq_entry_28, 28);
    REGISTER_idt_entry(irq29d_desc, _irq_entry_29, 29);
    REGISTER_idt_entry(irq30d_desc, _irq_entry_30, 30);
    REGISTER_idt_entry(irq31d_desc, _irq_entry_31, 31);
    
    REGISTER_idt_entry(irq255d_desc,_irq_entry_255, 255);

    struct IDT_ptr_t IDTptr  = { \
        .size = (sizeof( struct IDT_entry_t) * IDTable_sz) -1 , \
        .base = (u64_t)&IDT};
    
    _idt_init((u64_t)&IDTptr);

    //_outb(0x21, 0xfd);
    //_outb(0xa1, 0xff);
        
}