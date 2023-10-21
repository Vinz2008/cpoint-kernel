TARGET=i686-elf
CC=clang --target=i686-pc-none-elf -march=i686 -v
AS=$(TARGET)-as
CPOINT=cpoint -target-triplet i686-pc-none -no-std -no-gc

all: clean kernel_cpoint.bin

kernel_cpoint.bin: boot.o c/utils.o cpoint.o
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

cpoint.o:
	$(CPOINT) -o $@ kernel.cpoint -c

kernel_c.bin: boot.o kernel.o
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

boot.o: boot.s
	$(AS) boot.s -o $@

c/utils.o: c/utils.c
	$(CC) -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel.o: kernel.c
	$(CC) -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f ./*.o ./*.bin ./*.iso ./c/*.o ./*.ll
	rm -rf isodir

check:
	grub-file --is-x86-multiboot kernel_c.bin

iso_c:
	mkdir -p isodir/boot/grub
	cp kernel_c.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o kernel.iso isodir

iso: kernel_cpoint.bin
	mkdir -p isodir/boot/grub
	cp kernel_cpoint.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o kernel.iso isodir

qemu_c: kernel_c.bin iso_c
	qemu-system-i386 -cdrom kernel.iso

qemu: iso
	qemu-system-i386 -cdrom kernel.iso