OUTPUT_FORMAT("binary")

MEMORY
{
	ori (rx) : org = 0x7d00, l = 0x800
	krn (rx) : org = 0x9000, l = 0x3000
	mem (rwx) : ORIGIN = 0xc000, LENGTH = 0x1000
}
SECTIONS
{
	.text : {boot.o(.text*) } > ori
	.text : {entry.o(.text*) } > krn
	.data : {*(.data*) } > mem
	.bss : {*(.bss*) } > mem
	.rodata : {*(.rodata*) } > mem
	.eh_frame : {*(.eh_frame) } > mem 
}
