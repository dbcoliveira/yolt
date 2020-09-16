#include "utils.h"
#include "cpuid.h"

bool enable_fpu_sse()
{
    bool fpu_sse = false;
    
    if (cpuid_feature(CPUID_EDX, CPU_flag_sse) &&
        cpuid_feature(CPUID_EDX, CPU_flag_sse2) &&
        cpuid_feature(CPUID_ECX, CPU_flag_sse3) &&
        cpuid_feature(CPUID_ECX, CPU_flag_ssse3) &&
        cpuid_feature(CPUID_ECX, CPU_flag_sse4_1) &&
        cpuid_feature(CPUID_ECX, CPU_flag_sse4_2) &&
        cpuid_feature(CPUID_ECX, CPU_flag_xsave)) 
        {

            // enable SSE
            __asm__ volatile("mov %cr4, %rcx\n\t"
                             "or $1 << 9, %rcx\n\t" // OSFXSR
                             "or $1 << 10, %rcx\n\t" // OSXMMEXCPT
                             "mov %rcx, %cr4");

        
            u32_t cr4 = _reg_cr4();            
            if (!(check_bit(cr4, 9) && check_bit(cr4, 10)))
                return fpu_sse;

            fpu_sse = disable_legacy_fpu();
        }

    return fpu_sse;
}

// disable legacy fpu and emmulation.
int disable_legacy_fpu()
{    
    __asm__ volatile("mov %cr0, %rcx\n\t"                             
                     "and $~(1 << 2), %rcx\n\t" // EM = 0
                     "or $1 << 1, %rcx\n\t" // MP = 1
                     "mov %rcx, %cr0");

    u32_t cr0 = _reg_cr0();      
    if ((check_bit(cr0, 1) && !check_bit(cr0, 2)))
        return true;
    return false;
}
