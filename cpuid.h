#ifndef _CPUID_H
#define _CPUID_H

enum CPU_cpuid_mode { CPUID_S = 1, CPUID_ECX, CPUID_EDX, CPUID_EXT_ECX, CPUID_EXT_EDX, CPUID_EXT = 0x80000001};

// ECX
enum CPU_feature_ECX {
    CPU_flag_sse3,   //	Prescott New Instructions-SSE3 (PNI)
    CPU_flag_pclmulqdq, //	PCLMULQDQ
    CPU_flag_dtes64, //	64-bit debug store (edx bit 21)
	CPU_flag_monitor, //	MONITOR and MWAIT instructions (SSE3)
	CPU_flag_ds_cpl, //	CPL qualified debug store
	CPU_flag_vmx,    //	Virtual Machine eXtensions
	CPU_flag_smx,    //	Safer Mode Extensions (LaGrande)
	CPU_flag_est,    //	Enhanced SpeedStep
    CPU_flag_tm2,    //	Thermal Monitor 2
	CPU_flag_ssse3,  //	Supplemental SSE3 instructions
	CPU_flag_cnxt_id, //	L1 Context ID
	CPU_flag_sdbg,   //	Silicon Debug interface
	CPU_flag_fma,    //	Fused multiply-add (FMA3)
    CPU_flag_cx16,   //	CMPXCHG16B instruction
	CPU_flag_xtpr,   //	Disable task priority messages
	CPU_flag_pdcm,   //	Perfmon & debug capability
    CPU_flag_pcid = 17,   //	Process context identifiers
    CPU_flag_dca,    //  Direct cache access for DMA writes
    CPU_flag_sse4_1, //	SSE4.1 instructions
    CPU_flag_sse4_2,	//  SSE4.2 instructions
    CPU_flag_x2apic, //	x2APIC
    CPU_flag_movbe,  //	MOVBE instruction
    CPU_flag_popcnt, //	POPCNT instruction
    CPU_flag_tsc_deadline, //	APIC implements one-shot operation using a TSC deadline value
    CPU_flag_aes,    //	AES instruction set
    CPU_flag_xsave,  //	XSAVE, XRESTOR, XSETBV, XGETBV
    CPU_flag_osxsave,// XSAVE enabled by OS
    CPU_flag_avx,    //	Advanced Vector Extensions
    CPU_flag_f16c,   //	F16C (half-precision) FP feature
    CPU_flag_rdrnd,  //	RDRAND (on-chip random number generator) feature
    CPU_flag_hypervisor, //	Hypervisor present (always zero on physical CPUs)
};

// EDX
enum CPU_feature_EDX {
    CPU_flag_fpu,    // Onboard x87 FPU
    CPU_flag_vme,    // Virtual 8086 mode extensions
    CPU_flag_de,     // Debugging extensions (CR4 bit 3)
    CPU_flag_pse,    //Page Size Extension
    CPU_flag_tsc,    //	Time Stamp Counter
    CPU_flag_msr,    // Model-specific registers
    CPU_flag_pae,    // Physical Address Extension	
    CPU_flag_mce,    // 	Machine Check Exception
    CPU_flag_cx8,    // CMPXCHG8 (compare-and-swap) instruction
    CPU_flag_apic,   //  Advanced Programmable Interrupt Controller
    CPU_flag_sep = 11, // 	SYSENTER and SYSEXIT instructions
    CPU_flag_mttr, 	// Memory Type Range Registers
    CPU_flag_pge,    // Page Global Enable bit in CR4
    CPU_flag_mca,    // Machine check architecture
    CPU_flag_cmov,   // Conditional move and FCMOV instructions
    CPU_flag_pat,    // Page Attribute Table
    CPU_flag_pse36,  // 36-bit page size extension
    CPU_flag_psn,    // Processor Serial Number
    CPU_flag_clfsh,  //	CLFLUSH instruction (SSE2)
    CPU_flag_ds = 21, // Debug store: save trace of executed jumps
    CPU_flag_acpi,   // Onboard thermal control MSRs for ACPI
    CPU_flag_mmx,    // MMX instructions
    CPU_flag_fxsr,   // 	FXSAVE, FXRESTOR instructions
    CPU_flag_sse,    // SSE instructions (Katmai)
    CPU_flag_sse2,   // SSE2 instructions
    CPU_flag_ss,     // self-snoop cpu cache
    CPU_flag_htt,    // Hyper-threading
    CPU_flag_tm,     // Thermal monitor automatically limits temperature
    CPU_flag_ia64,   //	IA64 processor emulating x86
    CPU_flag_pbe,    // Pending Break Enable (PBE# pin) wakeup capability
};

// EDX
enum CPU_ext_featureS0 {
    /* already defined.
    .
    .
    */ 
    CPU_flag_syscall = 11,
    CPU_flag_mtrr,
    //CPU_flag_pge, already defined
    // CPU_flag_mca, already defined
    // CPU_flag_cmov, already defined
    //CPU_flag_pat, already defined
    //CPU_flag_pse36, already defined

    CPU_flag_mp = 19,
    CPU_flag_nx,
    CPU_flag_mmxext = 22,
    // CPU_flag_mmx, already defined
    // CPU_flag_fxsr, already defined
    CPU_flag_fxsr_opt = 25,
    CPU_flag_pdpe1gb,
    CPU_flag_rdtscp,

    CPU_flag_lm = 29,    
    CPU_flag_3dnowext,
    CPU_flag_3dnow,
};

#endif