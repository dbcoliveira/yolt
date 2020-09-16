.extern kernel_main
.extern _system_halt
.globl longmode_entry
.globl _idt_init

.globl _kernel_virtaddr
.globl _kernel_pge_frame

.set _KERNEL_VIRTUALADDR, 0xc0000000

.section .data
_kernel_virtaddr: .quad _KERNEL_VIRTUALADDR
_kernel_pge_frame: .quad _KERNEL_VIRTUALADDR >> 22

_idt_ptr64:
    // save 3072 bytes for IDT (256 * 12 bytes)
    .word 0x0c00
    .long 0x0800
GDTstruct64:
    // null descriptor is mandated by intel
_null_descriptor64:
    .word 0x0000, 0x0000
    .byte 0x0, 0x0, 0x0, 0x0
_code_descriptor64:
    .word 0x0000, 0x0000
    .byte 0x00, 0x9a, 0xaf, 0x00        
_data_descriptor64:
    .word 0x0000, 0x0000
    .byte 0x00, 0x92, 0x00, 0x00
endGDTstruct64:
_gdt64_ptr:
    .word . - GDTstruct64 - 1
    .long GDTstruct64
// PML4T -> PDPT -> PDT -> PT
.globl _pt_base_address
_pt_base_address: 
    .long 0x1000
_paging_flags:
    /* 
       bit 0 - present
       bit 1 - r/w
       bit 2 - CPL-0
    */
    .byte 0x03
.code32
.section .text
    // prepare to enter into 64bit compatability mode.
longmode_entry:    
    // set cr3 to _paging_base_address - PML4T top table
    movl _pt_base_address, %edi
    movl %edi, %cr3
    // clear PML4T, PDPT, PDT, PTE tables 
    xorl %eax, %eax
    movl $4096, %ecx
    rep stosb

    // set EDI again to _paging_base_address
    mov %cr3, %edi 
    // PML4T > PDPT
    movl _pt_base_address, %ecx
    addl _paging_flags, %ecx
    addl $0x1000, %ecx
    // base_address + 0x1000 + 3(PRESENT | WR)
    movl %ecx , (%edi) 
    add $0x1000, %edi
    // PDPT > PDT
    addl $0x1000, %ecx
    // base_address + 0x2000 + 3(PRESENT | WR)
    movl %ecx , (%edi)
    add $0x1000, %edi
    // PDT > PT
    addl $0x1000, %ecx
    // base_address + 0x3000 + 3(PRESENT | WR)
    movl %ecx , (%edi)
    add $0x1000, %edi 

    lidtl (_idt_ptr64)

    // identity map for the first 2MB
    movl $0x3, %ebx
    // counter 512
    mov $0x200, %ecx 
_ptesetup:
    movl %ebx, (%edi)
    add $0x1000, %ebx
    add $0x8, %edi
    loop _ptesetup   
    
    // set lm bit EFER_MSR
    mov $0xc0000080, %ecx
    rdmsr
    or $1 << 8, %eax 
    wrmsr
    
    // enable PAE
    mov %cr4, %eax
    or $1 << 5, %eax
    mov %eax, %cr4
    
    // enable paging
    mov %cr0, %ecx    
    or $1 << 31, %ecx        
    mov %ecx, %cr0

    lgdt (_gdt64_ptr)

    // 0x08 = _code_descriptor64
    ljmpl $0x08 , $_entry64

_entry64:
.code64
    cli
	// _data_descriptor64
    mov $0x10, %rax 
	movq %rax, %ds
	movq %rax, %es
	movq %rax, %fs
	movq %rax, %gs
	movq %rax, %ss    

    // flush TLB
    mov %cr3, %rcx
    mov %rcx, %cr3

    movq $0x90000, %rbp
	movq %rbp, %rsp
    
    jmp kernel_main
    // do not return
    // in any case stop here.
    jmp _system_halt

_idt_init:
    // first argument
    movq %rdi, %rax    
    lidt (%eax)
    sti
    ret
