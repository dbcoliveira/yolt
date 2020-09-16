#include "utils.h"

typedef struct registers_64 
{
    u64_t ds;
    u64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    u64_t int_n, err;
    u64_t rip, cs, eflags, oldrsp, oldss;
} __attribute__((packed)) registers_64_t;

void isr_handler(registers_64_t regs)
{
    kprintf("\n-- Interrupt Service Routine --\n");
    
    
    kprintf("RAX=0x%8x RBX=0x%8x RCX=0x%8x RDX=0x%8x\n", regs.rax, regs.rbx, regs.rcx, regs.rdx);
    kprintf("RSI=0x%8x RDI=0x%8x RBP=0x%8x RSP=0x%8x\n", regs.rsi, regs.rdi, regs.rbp, regs.rsp);
    kprintf("RIP=0x%8x EFLAGS=0x%8x interrupt=0x%x error=0x%x\n", regs.rip, regs.eflags, regs.int_n, regs.err);
    kprintf("GDT CS=0x%4x DS=0x%4x\n", regs.cs, regs.ds);
    
    kernel_noproceed("Fault exception.");    
}