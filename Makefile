all: kernel

kernel: Obj/kasm.o Obj/kc.o Obj/system.o Obj/irq.o Obj/keyboard.o Obj/time.o Obj/mouse.o Obj/hd.o Obj/hdc.o Obj/pci.o Obj/vga.o Obj/paging.o
	ld -m elf_i386 -T link.ld -o kernel Obj/kasm.o Obj/kc.o Obj/system.o Obj/irq.o Obj/keyboard.o Obj/time.o Obj/mouse.o Obj/hd.o Obj/hdc.o Obj/pci.o Obj/vga.o Obj/paging.o

Obj/kasm.o: start.asm
	nasm -f elf32 start.asm -o Obj/kasm.o

Obj/kc.o: kernel.c
	gcc -m32 -c kernel.c -fno-builtin -fno-stack-protector -o Obj/kc.o

Obj/system.o: system.c
	gcc -m32 -c system.c -fno-builtin -fno-stack-protector -o Obj/system.o

Obj/irq.o: irq.c
	gcc -m32 -c irq.c -fno-builtin -fno-stack-protector -o Obj/irq.o

Obj/keyboard.o: keyboard.c
	gcc -m32 -c keyboard.c -fno-builtin -fno-stack-protector -o Obj/keyboard.o

Obj/time.o: time.c
	gcc -m32 -c time.c -fno-builtin -fno-stack-protector -o Obj/time.o

Obj/mouse.o: mouse.c
	gcc -m32 -c mouse.c -fno-builtin -fno-stack-protector -o Obj/mouse.o

Obj/hd.o: hd.asm
	nasm -f elf32 hd.asm -o Obj/hd.o

Obj/hdc.o: hd.c
	gcc -m32 -c hd.c -fno-builtin -fno-stack-protector -o Obj/hdc.o

Obj/pci.o: pci.c
	gcc -m32 -c pci.c -fno-builtin -fno-stack-protector -o Obj/pci.o

Obj/vga.o: vga.c
	gcc -m32 -c vga.c -fno-builtin -fno-stack-protector -o Obj/vga.o

Obj/paging.o: paging.c
	gcc -m32 -c paging.c -fno-builtin -fno-stack-protector -o Obj/paging.o
