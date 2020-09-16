.code16
.extern longmode_entry

.section .text
.globl _start, start
_start:
	jmp _boot
	.word 0xdead
	.word 0xbeef
	// reboot placeholder
_reboot:
	ljmp $0xffff, $0x0

_halt:
	hlt
	jmp _halt

_printstr:
    lodsb
    cmp $0x0, %al
    je _printdone
	mov $0xe, %ah
	mov $0xa, %bx
    int $0x10
    jmp _printstr

_printdone:
	ret

_diskfailure:
	mov $_diskerror, %si
	call _printstr	
	jmp stayhere

_SMAPfailure:
	mov $_smaperror, %si
	call _printstr	
	jmp stayhere

_boot:	
	cli
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	
	mov $_start, %bp
	mov %bp , %sp 
	/* clear flags */
	pushl $0
	popfl

	mov $_bootstr, %si		
    call _printstr
	
	// checks for memory size via BIOS e820 int 0x15
	// system address map
_SMAP_mem_probe:
	mov $(_bios_mem_map), %di
	mov $0xe820, %eax
	movw $1, %es:20(%di)
	// 24
	mov $(8*3) , %ecx 
	xor %bp, %bp
	xor %ebx, %ebx
	// SMAP signature
	mov (_SMAP_sig), %edx 
	int $0x15
	jc _SMAPfailure
	mov (_SMAP_sig), %edx
	cmp %edx, %eax
	jne _SMAPfailure
	test %ebx, %ebx
	je _SMAPfailure
	jmp _SMAP_fetch_data

_SMAP_main_cycle:
	mov $0xe820, %eax
	mov (_SMAP_sig), %edx
	movw $1, %es:20(%di)
	mov $(8 * 3), %ecx
	int $0x15
	mov (_SMAP_sig), %edx
	jc _SMAP_end

_SMAP_fetch_data:
	jcxz _SMAP_skip
	cmp $20, %ecx
	jbe _SMAP_empty
	testb $1, %es:20(%di)
	je _SMAP_skip

_SMAP_empty:
	mov %es:8(%di), %ecx
	or  %es:12(%di), %ecx
	jz _SMAP_skip
	inc %bp
	add $24, %di

_SMAP_skip:
	// end ?
	test %ebx, %ebx 
	jne _SMAP_main_cycle

_SMAP_end:
	mov %bp, (_bios_mem_count)
	clc
	// reset drives
	xor %ax,%ax
	// bootdrive
	mov $0x00, %dl 
	int $0x13
	jc _diskfailure

	// test if can enable A20 via BIOS
	mov $0x2403, %ax
	int $0x15
	// if not, try fast io port A20
	jc  _A20_portA 
	// enable 20A via BIOS
	mov $0x2401, %ax
	int $0x15
	jc _halt

	/* notify BIOS on the run taget: 
	   protected and long mode. */
	mov $0xec00, %ax
	// mixed mode target
	mov $0x0003, %bx 
	int $0x15
	//set gdt descriptors tables
	jmp _set_desTables 

_A20_portA:
	// enable A20 line via fast io port 0x92 (system control port A)
	in $0x92, %al
	or $0x2, %al
	out %al, $0x92

_set_desTables:
// ligdt needs to go.
	cli
	// fill with 0s
	mov $0x0, %ax 
	mov %ax, %es
	xor %di, %di	
	// set 2048 bytes for interrupts
	mov $0x800, %cx 
	rep stosb
	
	lidtl _idt_ptr
	lgdtl _gdt_ptr

_enablePE:
	// set protected mode to 1 PE bit on CR0
	mov %cr0, %eax
	or $1 << 0, %eax
	mov %eax, %cr0
	
	ljmpl $0x08, $_set_32bit_segments

_set_32bit_segments:
.code32	
	// _data_descriptor:
	movl $0x10, %eax 
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs
	movl %eax, %ss
		
	// try to set 64bit long mode
	jmp longmode_entry 
	
stayhere:
	cli
	hlt
	jmp stayhere
	// should never return here

GDTstruct:
_null_descriptor:
        .quad 0x0

_code_descriptor:
        .word 0xffff, 0x0
        .byte 0x00, 0x9c, 0xcf, 0x00

_data_descriptor:
        .word 0xffff, 0x0
        .byte 0x00, 0x93, 0xcf, 0x00	
endGDTstruct:

_gdt_ptr:
        // save 32 bytes = 4 descriptors (null, code, data, tss)
        .word (endGDTstruct - GDTstruct)
        .long GDTstruct

_idt_ptr:
        // save 2048 bytes for IDT (256 * 8 bytes)
        .word 0xc00
        .long 0x800
_bootstr:
        .asciz "yolt: Loading... "

_diskerror:
        .asciz "\nyolt: Disk failure. "

_smaperror:
        .asciz "\nyolt: BIOS SMAP couldn't be retrieved." 
// 'SMAP'
_SMAP_sig: .quad 0x534d4150 

.globl _bios_mem_count
_bios_mem_count:	
		.quad 0x0

.globl _bios_mem_map
_bios_mem_map:		
		// 24 entries
		.skip 24 * 24, 0x0

_padding:
.skip 510, 0x0
.word 0xaa55


