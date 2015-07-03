#include "Include/system.h"
#include "Include/keymap.h"

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
char last_char;
int char_counter = 0;
short keypress = 0;

void kb_enable(void){
	/* 0xFD is 11111101 (this means we want to enable IRQ1 - keyboard) */
	write_port(0x21,0xFD);
}

void keyboard_handler_main(void){
	unsigned char status;
	char keycode;

	write_port(0x20,0x20);
	
	status = read_port(0x64);

	if(status & 0x01){
		keycode = read_port(0x60);
		if(keycode < 0)
			return;
		else if(keycode==0x1C)
		{
			keypress = 1;
			last_char = keycode;	
		}
		else if(keycode==0x0E)
		{
			keypress = 1;
			last_char = keycode;
		}
		else{
			keypress = 1;
			last_char = keycode;
		}
	}
}

char str_getch(void){
	while(keypress==0){

	}
	keypress = 0;
	return last_char;
}

void str_input(char buffer[],int max_chars){
	char_counter = 0;
	while(last_char != 0x1C){
		char keycode = str_getch();
		if(keycode == 0x0E){
			if(char_counter > 0){
			char_counter--;
			buffer[char_counter] = 0;
			backspace();
			}
		} else {
			if(keycode != 0x1C){
			scr_char(keyboard_map[keycode]);
			buffer[char_counter] = keyboard_map[keycode];			
			}
			char_counter++;
		}
		if(char_counter==(max_chars-1))
		{
			buffer[char_counter] = '\0';
			return;
		}
	}
	scr_newline();
	buffer[char_counter-1] = '\0';
	last_char = 0;
	return;
}

char get_last_char(void){
	return last_char;
}
