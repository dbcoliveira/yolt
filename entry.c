// in .code64 
#include "utils.h"
#include "cpuid.h"
#include "alloc.h"

#define YOLT_VERSION "alpha-0.0.1"
#define YOLT_VERSION_DATE "24 May 2020 GMT"
#define YOLT_VERSION_ARCH "x86_64"

#define knotify(info) kprintf("yolt: %s\n", info)

void kernel_noproceed(char *msg) {
	kprintf("\nyolt: %s - cannot proceed. halted.", msg); 	
    _system_halt();
}

bool enable_NX_protection() {
    if(cpuid_feature(CPUID_EXT_EDX, CPU_flag_nx))
    {
        u16_t EFER_MSR = r_EFER_MSR_base();
        
        if (!check_bit(EFER_MSR, 11))
            w_EFER_MSR_base(set_bit(EFER_MSR, 11, true));
        else
            return true;                
        if (check_bit(r_EFER_MSR_base(), 11))
            return true;
    }
    return false;        
}

static char *kernel_version() {
    return "YOLTk " YOLT_VERSION " " YOLT_VERSION_DATE " " YOLT_VERSION_ARCH "\n";
}

void kernel_main() {
    clearconsole();

    kprintf(kernel_version());

    // confirm if we arrived in 64bit long mode.    
    u16_t EFER_MSR = r_EFER_MSR_base();
    if (check_bit(EFER_MSR, 8) && check_bit(EFER_MSR, 10))    
        knotify("long mode enabled.");
    else
        kernel_noproceed("failed to initialise long mode.");

    setupIDT();

    // if available enable no-exec
    if (cpuid_feature(CPUID_EXT_EDX, CPU_flag_nx))
        if (enable_NX_protection())
            knotify("no-execute protection enabled.");

    // enable SSE and disable FPU emulation
    if (enable_fpu_sse())
        knotify("sse fpu enabled.");
    else
        kernel_noproceed("failed to initialise SSE.");
    
    // try to enable x2apic
    if (enable_x2APICmode())
        knotify("x2apic mode enabled.");
    else
        kernel_noproceed("failed to initialise x2apic.");

    if (cpuid_feature(CPUID_EDX, CPU_flag_htt))
            knotify("hyperthreading.\n");

    e820_SMAP_print();
   
    cpuid_getinfo();

    setup_paging();

    testa();
    __asm__ volatile ("int $0x3");   
/*
    kmalloc_init();
    //char *r = kmalloc(0x20);
    char *f = kmalloc(400);
    char *e = kmalloc(400);
    e = " ";
    char *a = kmalloc(0x10);
    a = "my variable test";
    kprint("\n");
    kprint(itoa(a,16));
    kprint("\n");
    kprint(itoa(e,16));
    */
/*
    for( int i = 0; i < 10; i++ ) {
        kprint("\n");
        //kmalloc(sizeof(char));
        kprint(itoa(kmalloc(0x10),16));
        kprint(",");
        kprint(itoa(kheap->alloc_count,10));        
    }
*/
    extern u64_t _kernel_virtaddr;
    extern u64_t _kernel_pge_frame;
    kprintf("\n0x%x",_kernel_virtaddr);
    kprintf("\n0x%x",_kernel_pge_frame);
    
    kernel_noproceed("end.");
}