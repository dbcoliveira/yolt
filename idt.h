#ifndef _IDT_H
#define _IDT_H

 struct IDT_entry_t {
    u16_t l_addr;
    u16_t GDT_CS_selector;
    u8_t reserved;
    u8_t type_attr;
    u16_t m_addr;
    u32_t h_addr;
}  __attribute__((packed));

struct IDT_ptr_t {
    u16_t size;
    u64_t base;
} __attribute__((packed));

    extern void _irq_entry_0();
    extern void _irq_entry_1();
    extern void _irq_entry_2();
    extern void _irq_entry_3();
    extern void _irq_entry_4();
    extern void _irq_entry_5();
    extern void _irq_entry_6();
    extern void _irq_entry_7();
    extern void _irq_entry_8();
    extern void _irq_entry_9();
    extern void _irq_entry_10();
    extern void _irq_entry_11();
    extern void _irq_entry_12();
    extern void _irq_entry_13();
    extern void _irq_entry_14();
    extern void _irq_entry_15();
    extern void _irq_entry_16();
    extern void _irq_entry_17();
    extern void _irq_entry_18();
    extern void _irq_entry_19();
    extern void _irq_entry_20();
    extern void _irq_entry_21();
    extern void _irq_entry_22();
    extern void _irq_entry_23();
    extern void _irq_entry_24();
    extern void _irq_entry_25();
    extern void _irq_entry_26();
    extern void _irq_entry_27();
    extern void _irq_entry_28();
    extern void _irq_entry_29();
    extern void _irq_entry_30();
    extern void _irq_entry_31();
    extern void _irq_entry_255();
#endif _IDT_H
