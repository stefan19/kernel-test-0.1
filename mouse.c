#include "Include/mouse.h"
#include "Include/system.h"
#include "Include/types.h"

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
u8int mouse_cycle = 0;
signed char mouse_byte[3];
signed char mouse_x=0;
signed char mouse_y=0;
u8int x_negative=0;
u8int y_negative=0;
int new_x = 0;

void mouse_handler_main(void){
	write_port(0x20,0x20);
	write_port(0xA0,0x20);

	u8int status = read_port(0x64);
	if( ((status & 0x01) == 1) && ((status & 0x20) == 0x20) ){
		switch(mouse_cycle)
		{
			case 0:
				mouse_byte[0]=read_port(0x60);
				if((mouse_byte[0] & 0x01) == 1)
					scr_char('l');
				if((mouse_byte[0] & 0x02) == 2)
					scr_char('r');
				if((mouse_byte[0] & 0x04) == 4)
					scr_char('m');
				if((mouse_byte[0] & 0x10) == 0x10)
					x_negative = 1;
				if((mouse_byte[0] & 0x20) == 0x20)
					y_negative = 1;
				mouse_cycle++;
				break;
			case 1:
				mouse_byte[1]=read_port(0x60);
				if(x_negative == 1)
					mouse_byte[1] = mouse_byte[1] | 0xFFFFFF00;
				mouse_x = mouse_byte[1];
				mouse_cycle++;
				break;
			case 2:
				mouse_byte[2]=read_port(0x60);
				if(y_negative == 1)
					mouse_byte[2] = mouse_byte[2] | 0xFFFFFF00;
				mouse_y=mouse_byte[2];
				mouse_cycle=0;
				break;			
		}
	}
	
}

void mouse_enable(void){
	write_port(0x21,0xF9);
	write_port(0xA1,0xEF);
}

inline void mouse_wait(u8int a_type){
	u32int _time_out = 100000;
	if(a_type == 0){
		while(_time_out--)
		{
		if((read_port(0x64) & 1)==1)
			{
			return;
			}
		}
		return;	
	} else {
		while(_time_out--)
		{
			if((read_port(0x64) & 2) == 0)
			{
				return;
			}
		}
		return;
	}
}

inline void mouse_write(u8int a_write){
	mouse_wait(1);
	write_port(0x64,0xD4);

	mouse_wait(1);
	write_port(0x60,a_write);		
}

u8int mouse_read(){
	mouse_wait(0);
	return read_port(0x60);
}

void mouse_install(void)
{
	u8int _status;

	mouse_wait(1);
	write_port(0x64, 0xA8);

	mouse_wait(1);
	write_port(0x64, 0x20);
	mouse_wait(0);
	_status=(read_port(0x60)| 2);
	mouse_wait(1);
	write_port(0x64,0x60);
	mouse_wait(1);
	write_port(0x60, _status);

	mouse_write(0xF6);
	mouse_read();

	mouse_write(0xF4);
	mouse_read();

	mouse_enable();
}

signed char mouse_movement_x(void){
	return mouse_x;
}

signed char mouse_movement_y(void){
	return mouse_y;
}
