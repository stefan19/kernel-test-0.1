#include "Include/irq.h"
#include "Include/system.h"
#include "Include/types.h"

extern void load_idt(u32int idt_ptr);
extern void keyboard_handler(void);
extern void mouse_handler(void);
extern void ide_handler(void);
extern void pagefault_handler(void);
extern void double_fault_handler(void);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void init_idt(){
	idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base = (u32int)&idt_entries;

	/*initialized the idt limit and base*/

	memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

	//IRQ 1 - Keyboard

	u32int keyboard_address = (u32int)keyboard_handler;
	idt_entries[0x21].base_lo = keyboard_address & 0xffff;
	idt_entries[0x21].base_hi = (keyboard_address >> 16) & 0xffff;
	
	idt_entries[0x21].sel = 0x08;
	idt_entries[0x21].always0 = 0;
	
	idt_entries[0x21].flags = 0x8E;

	//Trap 14 - Page fault
	u32int fault_address = (u32int)pagefault_handler;
	idt_entries[0x0D].base_lo = fault_address & 0xffff;
	idt_entries[0x0D].base_hi = (fault_address >> 16) & 0xffff;
	
	idt_entries[0x0D].sel = 0x08;
	idt_entries[0x0D].always0 = 0;
	
	idt_entries[0x0D].flags = 0x1F;

	//Trap 8 - Double fault
	u32int double_fault = (u32int)double_fault_handler;
	idt_entries[0x08].base_lo = double_fault & 0xffff;
	idt_entries[0x08].base_hi = (double_fault >> 16) & 0xffff;
	
	idt_entries[0x08].sel = 0x08;
	idt_entries[0x08].always0 = 0;
	
	idt_entries[0x08].flags = 0x1F;

	//IRQ 12 - Mouse

	//u32int mouse_address = (u32int)mouse_handler;	
	//idt_entries[0x2C].base_lo = mouse_address & 0xffff;
	//idt_entries[0x2C].base_hi = (mouse_address >> 16) & 0xffff;
	
	//idt_entries[0x2C].sel = 0x08;
	//idt_entries[0x2C].always0 = 0;
	
	//idt_entries[0x2C].flags = 0x8E;

	load_idt((u32int)&idt_ptr);  //finally load the IDT by assembly function

	write_port(0x20,0x11); //ICW1 - begin PIC initialization
	write_port(0xA0,0x11);

	write_port(0x21,0x20); //ICW2 - remap offset address of IDT
	write_port(0xA1,0x28);

	write_port(0x21,0x04); //ICW3 - setup cascading
	write_port(0xA1,0x02);

	write_port(0x21,0x01); //ICW4 - environment info
	write_port(0xA1,0x01);

	write_port(0x21,0xFF);
	write_port(0xA1,0xFF);
}

void pagefault_handler_main(void){
	scr_write("FATAL EXCEPTION: Page fault");
}

void double_fault_handler_main(void){
	scr_write("PANIC: Double fault");
}
