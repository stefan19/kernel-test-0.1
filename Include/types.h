#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8int;
typedef unsigned short u16int;
typedef unsigned long u32int;

struct idt_entry_struct
{
	u16int base_lo;
	u16int sel;
	u8int always0;
	u8int flags;
	u16int base_hi;
}__attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
	u16int limit;
	u32int base;
}__attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

#endif
