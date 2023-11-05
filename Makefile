TARGET=i686-elf
CC=clang --target=i686-pc-none-elf -march=i686
AS=$(TARGET)-as
CPOINT=cpoint -target-triplet i686-pc-none -no-std -no-gc


SRCDIR_ASM=asm

KERNEL_BIN=kernel_cpoint.bin


SRCDIR_KERNEL = kernel
SRCS := $(wildcard $(SRCDIR_KERNEL)/*.cpoint)
OBJS = $(patsubst $(SRCDIR_KERNEL)/%.cpoint,$(SRCDIR_KERNEL)/%.o,$(SRCS))

$(info OBJS=[$(OBJS)])

all: clean libc $(KERNEL_BIN)

libc/libk.a:
	make -C libc

$(KERNEL_BIN): $(SRCDIR_ASM)/crti.o $(SRCDIR_ASM)/boot.o $(SRCDIR_ASM)/gdt.o c/utils.o $(OBJS) libc/libk.a $(SRCDIR_ASM)/crtn.o
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

$(SRCDIR_KERNEL)/%.o: $(SRCDIR_KERNEL)/%.cpoint
	$(CPOINT) -o $@ $^ -c

#kernel_c.bin: boot.o kernel.o
#	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

$(SRCDIR_ASM)/boot.o: $(SRCDIR_ASM)/boot.s
	$(AS) $^ -o $@

$(SRCDIR_ASM)/crti.o: $(SRCDIR_ASM)/crti.s
	$(AS) $^ -o $@

$(SRCDIR_ASM)/crtn.o: $(SRCDIR_ASM)/crtn.s
	$(AS) $^ -o $@

$(SRCDIR_ASM)/gdt.o: $(SRCDIR_ASM)/gdt.s
	$(AS) $^ -o $@

c/utils.o: c/utils.c
	$(CC) -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

#kernel.o: kernel.c
#	$(CC) -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f ./kernel/*.o ./*.bin ./*.iso ./c/*.o ./asm/*.o
	rm -rf isodir
	make -C libc clean

check:
	grub-file --is-x86-multiboot $(KERNEL_BIN)

#iso_c:
#	mkdir -p isodir/boot/grub
#	cp kernel_c.bin isodir/boot/kernel.bin
#	cp grub.cfg isodir/boot/grub/grub.cfg
#	grub-mkrescue -o kernel.iso isodir

iso: $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o kernel.iso isodir

#qemu_c: kernel_c.bin iso_c
#	qemu-system-i386 -cdrom kernel.iso

qemu: iso
	qemu-system-i386 -cdrom kernel.iso -serial file:logs.txt