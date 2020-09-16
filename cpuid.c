#include "utils.h"
#include "cpuid.h"

// num of registers where information is stored from cpuid {0 = rax, 1 = rbx, 2 = rcx, 3 = rdx}
#define cpuidnr 4
#define CPUID_adv_base_param 0x80000000

const char *CPU_ext_featureS0Str[32] = {
    "dup", "dup", "dup", "dup", "dup", "dup", "dup", "dup",
    "dup", "dup", "reserved_10", "syscall", "mtrr", "pge", "dup", "cmov",
    "pat", "pse36", "reserved_18", "mp", "nx", "reserved_21", "mmxext", "dup", 
    "fxsr", "fxsr_opt", "pdpe1gb", "rdtscp", "reserved_28", "lm", "3dnowext", "3dnow",
};

const char *CPU_feature_ECXStr[32] = {
    "sse3", "pclmulqdq", "dtes64", "monitor", "ds_cpl", "vmx", "smx", "est", 
	"tm2", "ssse3", "cnxt_id", "sdbg", "fma", "cx16", "xtpr", "pdcm",   
    "reserved_16", "pcid", "dca", "sse4_1","sse4_2","x2apic", "movbe", "popcnt", 
    "tsc_deadline", "aes", "xsave", "osxsave", "avx", "f16c", "rdrnd", "hypervisor" 
};

const char *CPU_feature_EDXStr[32] = {
    "fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
    "cx8", "apic", "reserved_10", "sep", "mttr", "pge", "mca", "cmov",
    "pat", "pse36", "psn", "clfsh", "reserved_20", "ds", "acpi", "mmx",
    "fxsr", "sse", "sse2", "ss", "htt", "tm", "ia64" "pbe",
};

u32_t _reg_cr4()
{
	u32_t ecx;
	__asm__ volatile ("mov %%cr4,%%rcx"
			  : "=c" (ecx):);
	return ecx;
}

u32_t _reg_cr0()
{
	u32_t ecx;
	__asm__ volatile ("mov %%cr0,%%rcx"
			  : "=c" (ecx):);
	return ecx;
}

void cpuid(u32_t idx, u64_t reg[]) 
{
	__asm__ volatile ("cpuid"
			  : "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3])
			  : "a" (idx));
}

const char* cpuid_getvendorname() {
        // 1 cpuid * 3 registers * 4 bytes
		static char vendorid[16];
		u64_t reg[cpuidnr];
		
        cpuid(0, reg);
		
		u64_t tmpreg = {reg[2]};
		reg[2] = reg[3];
		reg[3] = tmpreg;

		u8_t i ,s;
        for( s = 0 , i=1 ; i < cpuidnr; i++)
            for(u8_t b = 0 ; b < sizeof (u32_t); b++)
                vendorid[s++]= (char*)((reg[i] >> (8*b)) & 0xff);

	    return vendorid;
}

const char* cpuid_getbrandindex()
{
		// 3 cpuid's * 4 registers * 4 bytes 
        static char brandindex[3 * 4 * 4];
		u64_t reg[cpuidnr];

		u8_t s, r;
		for (s = 0, r = 2; r <= 4; r++) 
		{
			cpuid(CPUID_adv_base_param+r, reg);
        	for(int i = 0 ; i < cpuidnr; i++)
                for(int b = 0 ; b < sizeof (u32_t); b++)
                    brandindex[s++]= (char*)((reg[i] >> (8*b)) & 0xff);
		}

       return brandindex;
}

bool cpuid_feature(u64_t cpuidmode , unsigned char feature)
{
	u64_t reg[cpuidnr];
	
	switch (cpuidmode) {
		case CPUID_EDX:
			cpuid(CPUID_S, reg);
			return check_bit(reg[3], feature); // rdx
		case CPUID_ECX:
			cpuid(CPUID_S, reg);
			return check_bit(reg[2], feature); // rcx
		case CPUID_EXT_EDX:
			cpuid(CPUID_EXT, reg);
			return check_bit(reg[3], feature); // rdx
		case CPUID_EXT_ECX:
			cpuid(CPUID_EXT, reg);
			return check_bit(reg[2], feature); // rcx
		default:
			return false;
	}
}

void cpuid_getinfo()
{
	static u64_t reg[cpuidnr];

	cpuid(0x1, reg);
	u16_t stepping = reg[0] & 0x0000000f;
    u16_t model = reg[0] & 0x000000f0;
	u16_t family =  ((( 1 << 12)-1) & (reg[0] >> (9 - 1)));
	u16_t cputype = ((( 1 << 14)-1) & (reg[0] >> (13 - 1)));
	
	//const char *hard = cpuid_getvendorname();
	kprintf("yolt: Hardware: %s, %s\n", cpuid_getvendorname(), cpuid_getbrandindex());
	kprintf("yolt: Stepping: %d, Model: %d, Family: %d, Cputype: %d\n", stepping, model, family, cputype);
	kprintf("yolt: CPUID flags: ");

	for (u8_t i = 0 ; i <= 31; i++) {
		if (check_bit(reg[3], i)) 
			kprintf("%s ", CPU_feature_EDXStr[i]);		
		if (check_bit(reg[2], i)) 
			kprintf("%s ", CPU_feature_ECXStr[i]);		
	}

	cpuid(CPUID_adv_base_param + 1, reg);
	for (u8_t i = 0 ; i <= 31; i++)
		if (check_bit(reg[3], i)) 
			kprintf("%s ", CPU_ext_featureS0Str[i]);			
}
