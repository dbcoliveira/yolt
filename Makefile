# QEMU KVM setup
CPU = 4
MEM = 1024
MONITOR = -monitor stdio
KVM = -enable-kvm
CPU_TYPE = host

# OBJ_C order, matters for ld
OBJ_C = entry.o video.o cpuid.o utils.o x2apic.o msr.o idt.o isrhandler.o fpu_sse.o smap.o acpi.o vm.o alloc.o printf.o
OBJ_S = boot.o longm.o irq.o

LDMAP = out.map
IMGISO = yolt.iso
OUTIMAGE = yimage

CC = gcc
CFLAGS = -g -w -mno-red-zone -Os -fno-stack-protector -ffreestanding
LDFLAGS = -Map $(LDMAP) -nostdlib -Tdata=0x30000 -Ttext=0x7c00 --oformat binary

SRC_C = $(OBJ_C:.o=.c)
SRC_S = $(OBJ_S:.o=.s)

$(OBJ_C): $(SRC_C)
	gcc $(CFLAGS) -c $^

$(OBJ_S): $(SRC_S)
	gcc -c $^

kernel: $(OBJ_S) $(OBJ_C)
	ld  $(LDFLAGS) -o $(OUTIMAGE) $^

image: kernel
	mkisofs -no-emul-boot -quiet -V 'YOLT' -o $(IMGISO) -b $(OUTIMAGE) ./

run:
	qemu-system-x86_64 -smp $(CPU) -cpu $(CPU_TYPE) $(KVM) -s -S -boot d -cdrom $(IMGISO) --m $(MEM) $(MONITOR)

clean:
	rm *.o
	rm $(IMGISO) $(OUTIMAGE) $(LDMAP)
