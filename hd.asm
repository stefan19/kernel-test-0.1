section .text

global read_hd
global read_hd_lba
extern sector
extern cylinder_lo
extern cylinder_hi
extern drive_head
extern lba

read_hd:
	push edx
	push eax
	push edi

	mov dx,1f6h		;Drive and head
	mov al,[drive_head]
	out dx,al

	mov dx,1f2h
	mov al,1		;Sector count port - 1
	out dx,al

	mov dx,1f3h
	mov al,[sector]		;Sector number port - First argument	
	out dx,al

	mov dx,1f4h		
	mov al,[cylinder_lo]
	out dx,al

	mov dx,1f5h
	mov al,[cylinder_hi]
	out dx,al

	mov dx,1f7h
	mov al,20h
	out dx,al
	
	.still_going:
	in al,dx
	test al,8
	jz .still_going

	mov cx,512/2
	mov di,[harddisk_buffer]
	mov dx,1f0h
	rep insw

	pop edx
	pop eax
	pop edi
	
	mov eax,[harddisk_buffer]

	ret

read_hd_lba:
	push edx
	push eax
	push edi
	push ebx

	mov eax, [lba]
	mov ebx, eax
	
	mov edx,0x01F6
	shr eax,24
	or al,11100000b
	out dx,al

	mov edx,0x01F2
	mov al,1
	out dx,al

	mov edx,0x01F3
	mov eax,ebx
	out dx,al
	
	mov edx,0x1F4
	mov eax,ebx
	shr eax,8
	out dx,al
	
	mov edx,0x1F5
	mov eax,ebx
	shr eax,16
	out dx,al

	mov edx,0x1F7
	mov al, 0x20
	out dx,al

.still_going:
	in al,dx
	test al,8
	jz .still_going

	mov cx,256
	mov di,[harddisk_buffer]
	mov dx,1F0h
	rep insw

	pop edx
	pop eax
	pop edi
	pop ebx

	mov eax,[harddisk_buffer]
	ret
	
section .bss
harddisk_buffer resb 512
