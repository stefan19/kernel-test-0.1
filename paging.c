#include "Include/types.h"

extern void loadPageDirectory(u32int*);
extern void enablePaging();

void init_paging(){
	u32int page_directory[1024] __attribute__((aligned(4096)));
	int i;
	for(i = 0; i < 1024; i++)
	{
		page_directory[i] = 0x00000002;
	}
	u32int first_page_table[1024] __attribute__((aligned(4096)));
	for(i = 0; i < 1024; i++)
	{
		first_page_table[i] = (i * 0x1000) | 3;
	}
	page_directory[0] = ((unsigned int)first_page_table) | 3;
	loadPageDirectory(page_directory);
	enablePaging();
	scr_newline();
	scr_write("Paging is now enabled!");
}
