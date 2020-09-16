#include "utils.h"

#define EFER_MSR_BASE_ADDR 0xc0000080

void _wrmsr(u32_t msrReg, u32_t high, u32_t low)
{
    __asm__ volatile ("mfence\n\t"
                        "wrmsr\n\t"
                    :: "d" (high), "a" (low), "c" (msrReg));
}

u64_t _rdmsr(u32_t msrReg)
{
    u32_t high, low;
    
    __asm__ volatile ("rdmsr\n\t"
                    : "=d" (high), "=a" (low) : "c" (msrReg));

    return high << 32 | low;
    return 0;
}

u64_t r_EFER_MSR_base()
{
    return _rdmsr(EFER_MSR_BASE_ADDR);            
}

u64_t w_EFER_MSR_base(u64_t reg)
{
    _wrmsr(EFER_MSR_BASE_ADDR, reg & 0xffffffff00000000, reg & 0x00000000ffffffff);            
}
