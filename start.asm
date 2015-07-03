bits 32

section .text
	align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)

global start
global keyboard_handler
global mouse_handler
global ide_handler
global pagefault_handler
global read_port
global write_port
global load_idt
global inl
global outl
global inw
global loadPageDirectory
global enablePaging
global double_fault_handler
global start_paging

extern kmain
extern keyboard_handler_main
extern mouse_handler_main
extern ide_handler_main
extern pagefault_handler_main
extern double_fault_handler_main

read_port:
	mov edx, [esp + 4]
	in al,dx
	ret

write_port:
	mov edx, [esp + 4]
	mov al, [esp + 4 + 4]
	out dx,al
	ret

inl:
	mov edx, [esp + 4]
	in eax,dx
	ret

outl:
	mov edx, [esp + 4]
	mov eax, [esp + 4 + 4]
	out dx,eax
	ret

inw:
	mov dx, [esp + 4]
	in ax,dx
	ret

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

keyboard_handler:
	call keyboard_handler_main
	iretd

mouse_handler:
	call mouse_handler_main
	iretd

ide_handler:
	call ide_handler_main
	iretd

pagefault_handler:
	call pagefault_handler_main
	hlt

double_fault_handler:
	call double_fault_handler_main
	hlt

loadPageDirectory:
push ebp
mov ebp, esp
mov eax, [esp + 8]
mov cr3, eax
mov esp, ebp
pop ebp
ret

enablePaging:
push ebp
mov ebp, esp
mov eax, cr0
or eax,0x80000000
mov cr0, eax
mov esp, ebp
pop ebp
ret

start_paging:
push ecx
push edx
push eax

mov ecx, 2048
mov edx, 0x9C000
.ZeroMemoryLoop:
mov dword [edx], 0
add edx, 4
loop .ZeroMemoryLoop

mov dword [0x9C000], 0x9D003

mov ecx, 1024
mov edx, 0x9D000
mov eax, 11b
.GenTable:
mov dword [edx], eax
add edx, 4
add eax,0x1000
loop .GenTable

mov eax, 0x9C000
mov cr3, eax

mov eax, cr0
or eax, 0x80000000
mov cr0, eax

start:
	cli
	mov esp, stack_space
	call kmain
	hlt

section .bss
resb 8192
stack_space:
