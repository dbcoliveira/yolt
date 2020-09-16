#include "utils.h"
#include "cpuid.h"
#include "acpi.h"
#include "vm.h"

#define MAXCPUS 0xff

#define PIC_8259_SLAVE_DATA 0xa1
#define PIC_8259_MASTER_DATA 0x21

// manual https://www.naic.edu/~phil/software/intel/318148.pdf

#define IA32_APIC_BASE_ADDR 0x1b
#define IA32_APIC_BASE_BSP 0x8

#define X2APIC_OFFSET 0x800
#define LOCAL_X2APIC_ID_REG 0x02
#define LOGICAL_X2APIC_ID_REG 0x0d
#define LOCAL_X2APIC_VERSION_REG 0x03
#define SVR_X2APIC_REG 0x0f
#define LOCAL_X2API_ICR 0x30
#define LOCAL_X2API_ESR 0x28

enum interrupt_ctl_types { processor_local_apic, io_apic, interrupt_source_override, nmi_source, local_apic_nmi };

struct cpuLocalAPIC {
    u32_t flags;
    u8_t apic_ID;
    u8_t apic_cpu_UID;
    u8_t length;
    u8_t type;    
};

struct IOAPIC {
    u32_t gsib; /* global system interrupt base */
    u32_t io_apic_addr;
    u8_t reserved;
    u8_t io_apic_ID;
    u8_t length;
    u8_t type;    
};

bool x2APICmode = false;

u64_t r_IA32apic_base() {
    return _rdmsr(IA32_APIC_BASE_ADDR);            
}

void w_IA32apic_base(u64_t reg) {
    return _wrmsr(IA32_APIC_BASE_ADDR, reg & 0xffffffff00000000, reg & 0x00000000ffffffff);
}

u64_t r_LOCALVersion_x2APIC_reg() {
    return _rdmsr(X2APIC_OFFSET + LOCAL_X2APIC_VERSION_REG);            
}

u32_t r_LOCAL_x2APIC_ID_reg() {
    return _rdmsr(X2APIC_OFFSET + LOCAL_X2APIC_ID_REG);            
}

u32_t r_LOGICAL_x2APIC_ID_reg() {
    return _rdmsr(X2APIC_OFFSET + LOGICAL_X2APIC_ID_REG);            
}

u64_t r_SVR_x2APIC_reg() {
    return _rdmsr(X2APIC_OFFSET + SVR_X2APIC_REG);
}

u64_t w_SVR_x2APIC_reg(u64_t reg) {
    return _wrmsr(X2APIC_OFFSET + SVR_X2APIC_REG, reg & 0xffffffff00000000, reg & 0x00000000ffffffff);
}

bool x2APIC_isBSP() {
    return check_bit(r_IA32apic_base(), IA32_APIC_BASE_BSP);
}

bool enable_x2APICmode() {
    if (cpuid_feature(CPUID_ECX, CPU_flag_x2apic)) {
            u64_t IA32_APIC_BASE = r_IA32apic_base();         
            if (check_bit(IA32_APIC_BASE, 11)) {
                // disable 8259PIC
                PIC_8259A_shutdown();                
                // enable x2apic
                w_IA32apic_base(set_bit(IA32_APIC_BASE, 10, true));                
                IA32_APIC_BASE = r_IA32apic_base();
                // check EXTD and EN bits
                if (check_bit(IA32_APIC_BASE, 10) && check_bit(IA32_APIC_BASE, 11))
                    x2APICmode = true;
            }
            else
                x2APICmode = false;
    }
    return x2APICmode;
}

// disable 8259PIC.
void PIC_8259A_shutdown() {
    _outb(PIC_8259_SLAVE_DATA, 0xff);
    _outb(PIC_8259_MASTER_DATA, 0xff);
}

bool cpuEnabled(struct cpuLocalAPIC *clapic) {
    if (clapic->type == processor_local_apic)
        if (check_bit(clapic->flags, 0)) 
            return true;
    return false;
}

int loadLocalAPIC(struct acpi_RSDheader *madt, struct cpuLocalAPIC clapic[]) {       
    if (strncmp(madt->rsd_signature, MADT_SIG, strlen(MADT_SIG) ) != 0)
        return -1;

    u64_t *b = (u32_t*) madt->entry;
    u8_t n = 0;

    while (((*b >> 32) & 0xff) == processor_local_apic) 
    {        
        clapic[n].apic_cpu_UID = (*b >> 48) & 0xff;
        clapic[n].apic_ID = (*b >> 56) & 0xff;    
        clapic[n].length = (*b >> 40) & 0xff;    
        clapic[n].type = (*b >> 32) & 0xff;
        clapic[n].flags = *b & 0xffffffff;        
        n++; b++;
    }
    return 0;   
}

int numCPULocalAPIC(struct acpi_RSDheader *madt) {       
    u64_t *b = (u32_t*) madt->entry;
    u8_t n = 0;

    if (strncmp(madt->rsd_signature, MADT_SIG, strlen(MADT_SIG) ) != 0)
        return -1;
    
    while (((*b++ >> 32) & 0xff) == processor_local_apic) 
        n++;

    return n;
}

int loadIOAPIC(struct acpi_RSDheader *madt, struct IOAPIC *ioapic) {
    if (strncmp(madt->rsd_signature, MADT_SIG, strlen(MADT_SIG) ) != 0)
        return -1;

    u64_t *b = (u32_t*) (madt->entry); // ((*madt->entry >> 40) & 0xff);        
    b += numCPULocalAPIC(madt);
    
    ioapic->type = (*b >> 32) & 0xff;
    ioapic->length = (*b >> 40) & 0xff;
    ioapic->io_apic_ID = (*b >> 48) & 0xff;
    ioapic->reserved = 0;
    ioapic->io_apic_addr = *(++b) & 0xffffffff;
    ioapic->gsib = (*b >> 32) & 0xffffffff;
    
    return 0;
}

void testa() {

    kprintf("\n  -- ACPI RSDptr\n");
    struct acpi_RSDptr *rsdp = NULL; 
    rsdp = acpi_RSDP_table(rsdp);
    kprintf("xsdt_addr: 0x%x, rsdt_addr: 0x%x, rsd_signature: %s", rsdp->xsdt_addr, rsdp->rsdt_addr, rsdp->rsd_signature);
    map_addr((u32_t*)rsdp->rsdt_addr);
    volatile struct acpi_RSDheader *rsdheader = rsdp->rsdt_addr;     
    volatile struct acpi_RSDheader *madt = (u32_t*) (rsdheader->entry[1]);     
    volatile struct u32_t *svradd = (u32_t*) 0xfee000f0;
    map_addr(svradd);
    map_addr(madt);

// test
    
    //map_addr(0xc0000000);
    map_addr(0xf1001000);
    map_addr(0xd199ff1f);
    map_addr(0xc0000000);

/*
    kprint("\n");
    kprint(rsdheader->rsd_signature[1]);
    kprint("\n");
    kprint(itoa(rsdheader->length,16));
    kprint("\n");
    kprint(itoa(rsdheader->revision,16));

    kprint("\n");
    kprint(rsdheader->OEM_ID);
    kprint("\n");
    kprint(rsdheader->OEM_TABLE_ID);

    kprint("\n");
    kprint(rsdheader->creator_ID);
    kprint("\n");
    kprint(itoa(rsdheader->entry,16));
    kprint("\n");
    kprint(itoa(sizeof(rsdheader->entry),16));
*/
    kprintf("\n  -- LOCAL APIC\n");

    // can change the SVR vector without problems.
    /*
    u32_t svr = r_SVR_x2APIC_reg();
    kprint("\nSVR = ");
    kprint(itoa(svr, 2));
    kprint("\nSVR = ");
    svr = set_bit(svr,5,0);
    kprint(itoa(svr, 16));
    w_SVR_x2APIC_reg(svr);
    */
    struct cpuLocalAPIC local_apic[16];
    
    loadLocalAPIC(madt, local_apic);
    
    int ncpu = numCPULocalAPIC(madt);
    /*
    kprint("\nnumber of CPUs: ");
    kprint(itoa(ncpu,16));

    for (int i = 0; i < ncpu; i++)
    {
        kprint("\n");
        kprint("  [ cpu_uid: ");
        kprint(itoa(local_apic[i].apic_cpu_UID,16));

        kprint(", apic_id: ");
        kprint(itoa(local_apic[i].apic_ID,16));        
        
        if (cpuEnabled(&local_apic[i]))
            kprint(" ] enabled");
        else 
            kprint(" ]");
    }
    kprint("\n  -- IOAPIC\n");
    */
    struct IOAPIC *ioapic;
    loadIOAPIC(madt, ioapic);

    kprintf("id: 0x%x, type: 0x%x, length: 0x%x, io_apic_addr: 0x%x, gsib: 0x%x\n", 
        ioapic->io_apic_ID,
        ioapic->type,
        ioapic->length,
        ioapic->io_apic_addr,
        ioapic->gsib);

    kprintf("yolt: boostrap processor: %d\n", x2APIC_isBSP());
   
    u64_t msrlapic = _rdmsr(0x800 + 0x30);    
    kprintf("%s -> ", itoa(msrlapic, 2));
    // vector at address 0x0f -> 0x0000f000

    msrlapic = set_bit(msrlapic, 8, 1);
    msrlapic = set_bit(msrlapic, 9, 0);
    msrlapic = set_bit(msrlapic, 10, 1);
    msrlapic = set_bit(msrlapic, 14, 0); // level : de-assert
    msrlapic = set_bit(msrlapic, 15, 1); // trigger mode: level

    kprintf("%s\n", itoa(msrlapic, 2));
    //kprint(itoa(msrlapic, 2));    
    
    kprintf("IIPI");
    _wrmsr(0x800+0x30, msrlapic);        
    
    u64_t msrlapic2 = _rdmsr(0x800 + 0x30);
    
    kprintf(itoa(msrlapic2, 2));


    msrlapic = set_bit(msrlapic, 8, 0);
    msrlapic = set_bit(msrlapic, 9, 1);
    msrlapic = set_bit(msrlapic, 10, 1);
    msrlapic = set_bit(msrlapic, 14, 1); // level : assert
    msrlapic = set_bit(msrlapic, 15, 0); // tigger mode: edge
    
    kprintf("\n");
    kprintf(itoa(msrlapic, 2));

    kprintf("\nSIPI");
    _wrmsr(0x800+0x30, msrlapic);
    kprintf("\nSIPI\n");
    _wrmsr(0x800+0x30, msrlapic);
/*
    w = 300000;
    while( w > 0 ){
        w--;
    };
*/
    memset(msrlapic2, 0, sizeof(msrlapic2));
    u64_t msrlapic3 = _rdmsr(0x800 + 0x28);   
    kprintf(itoa(msrlapic3, 2));    
    
    //kprint("\n");
    //u64_t svr = r_SVR_x2APIC_reg();
    //kprint(itoa(svr, 2));    
/*
    for (int i=0 ; i < 144; i++)
        pt_bitmap_set(i,true);

        pt_bitmap_set(33,true);
        pt_bitmap_set(8,true);

        pt_bitmap_set(11,true);
        pt_bitmap_set(71,false);
        pt_bitmap_set(71,true);

*/
      //pt_bitmap_set(0,true);
    for (int i=0 ; i < 32; i++)
        kprintf(itoa(pt_bitmap_get(i),10));
    flush_pt(0x73000);  
}