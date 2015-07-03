#include "Include/types.h"
#include "Include/system.h"
#include "Include/irq.h"
#include "Include/keyboard.h"
#include "Include/time.h"
#include "Include/mouse.h"
#include "Include/hd.h"

extern void mouse_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);

extern void str_write(char* str);
extern int current_location;
extern void init_idt();
extern void start_paging(void);

extern void kb_enable(void);
void console(void);
void gui(void);

void kmain(void){
	init_idt();
	kb_enable();
	//mouse_install();
	scr_clear();
	scr_write("System started!");
	//start_paging();
	console();
	while(1);
}

void console(void){
	char *prompt = ">";
	scr_newline();
	scr_write(prompt);
	char string[10];
	str_input(string,10);
	if(string[0] == 'v' && string[1] == 'e' && string[2] == 'r' && string[3] == '\0'){
	scr_write("Version 1.0; This x86 OS runs only in protected mode");
	}
	else if(string[0] == 'c' && string[1] == 'l' && string[2] == 's' && string[3] == '\0'){
	scr_clear();
	}
	else if(string[0] == 'h' && string[1] == 'e' && string[2] == 'l' && string[3] == 'p' && string[4] == '\0'){
	scr_write("You may use this commands:");
	scr_newline();
	scr_write("ver - Displays version number");
	scr_newline();
	scr_write("cls - Clears screen");
	scr_newline();
	scr_write("help - Displays this help");
	scr_newline();
	scr_write("time - Displays the current time");
	}
	else if(string[0] == 'i' && string[1] == 'n' && string[2] == 't' && string[3] == '\0'){
	print_int(123);
	}
	else if(string[0] == 't' && string[1] == 'i' && string[2] == 'm' && string[3] == 'e' && string[4] == '\0'){
	scr_write("Current time is: ");
	read_rtc();
	}
	else if(string[0] == 'd' && string[1] == 'i' && string[2] == 'r' && string[3] == '\0'){
	read_volume_info();
	dir();
	}
	else if(string[0] == 'f' && string[1] == 'i' && string[2] == 'l' && string[3] == 'e' && string[4] == '\0'){
	char name[] = {'F','I','L','E',' ',' ',' ',' ','T','X','T'};
	read_file(name);
	}
	else if(string[0] == 'h' && string[1] == 'd' && string[2] == '\0'){
		u32int result = checkAllBuses(0x01,0x01);
		if(result == 0xFFFFFF)
			scr_write("IDE Controller not found!");
		else
			scr_write("IDE Controller found!");
		u8int ideBus = (result >> 16) & 0xFF;
		u8int ideDevice = (result >> 8) & 0xFF;
		u8int ideFunc = result & 0xFF;
		u32int BAR4 = getBAR4(ideBus,ideDevice,ideFunc);
		u32int io_port = 0;
		if((BAR4 & 1) == 1)
			io_port = BAR4 & 0xFFFFFFFC;
		dma_read(io_port);
	}
	else if(string[0] == 'g' && string[1] == 'u' && string[2] == 'i' && string[3] == '\0'){
		gui();
	}
	else if(string[0] == '\0'){
	}
	else{
	scr_write("Bad command or filename");
	}
	console();
}

int str_cmp(char str1[],char str2[]){
	int i = 0;
	while(str1[i] != '\0' && str2[i] != '\0'){
		if(str1[i] == str2[i])
			i++;
		else
			return 0;
	}
	return 1;
}

void gui(void){
	init_screen();
	clear_screen(9);
	draw_rectangle(0,0,319,9,8);
	draw_char(1,1);	
}
	
void mouse_interface(){
	signed char x = 0;
	signed char y = 1;
	signed char last_x_move = 0;
	signed char last_y_move = 0;
	scr_clear();
	change_position(0,0);
	int i = 0;
	for(i=0;i<80;i++){
	color_char(' ',0x00,0x02);
	}
	change_position(0,1);
	color_char(' ',0x00,0x04);
	while(1){
		signed char x_mov = mouse_movement_x();
		if(x_mov != last_x_move){
			if(x+x_mov >= 80){
				x = 79;
			} else if(x+x_mov < 0){
				x = 0;
			}
			else{
				x += x_mov;
			}
			change_position(x,y);
			color_char(' ',0x00,0x04);
			change_position(x - x_mov,y);
			color_char(' ',0x07,0x00);
			last_x_move = x_mov;
		}

		signed char y_mov = mouse_movement_y();
		if(y_mov != last_y_move){
			if(y-y_mov >= 25){
				y = 24;
			} else if(y-y_mov < 1){
				y = 1;
			}
			else{
				y -= y_mov;
			}
			change_position(x,y);
			color_char(' ',0x00,0x04);
			change_position(x,y - y_mov);
			color_char(' ',0x07,0x00);
			last_y_move = y_mov;
		}
	}
}
