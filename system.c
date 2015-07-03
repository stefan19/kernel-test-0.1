#include "Include/system.h"
#include "Include/types.h"

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);

unsigned short current_location = 0;
char *vidptr = (char*)0xb8000;
void update_cursor(unsigned short position);
void scroll_down(void);

void *memset(void *s,char c,int n){
	unsigned char* p = s;
	while(n--)
		*p++ = (unsigned char)c;
}

void scr_write(char *str){
	if(current_location >= 80*25*2){
		scroll_down();
		current_location -= 80*2;
		int j=0;
		while(str[j] != '\0'){
			vidptr[current_location] = str[j];
			vidptr[current_location + 1] = 0x07;
			j++;
			current_location += 2;
		}
		update_cursor(current_location/2);
	} else {
	int j=0;
	while(str[j] != '\0'){
		vidptr[current_location] = str[j];
		vidptr[current_location + 1] = 0x07;
		j++;
		current_location += 2;
	}
	update_cursor(current_location/2);
	}
}

void scr_clear(void){
	int j=0;
	while(j < 80*25*2){
		vidptr[j] = ' ';
		vidptr[j+1] = 0x07;
		j += 2;
	}
	current_location = 0;
	update_cursor(current_location/2);
}

void scr_newline(void){
	if(current_location >= 80*25*2)
	{
		scroll_down();
		current_location -= 80*2;
		unsigned int line_size = 80 * 2;
		current_location = current_location + (line_size - current_location % (line_size));
		update_cursor(current_location/2);
	} else {
	unsigned int line_size = 80 * 2;
	current_location = current_location + (line_size - current_location % (line_size));
	update_cursor(current_location/2);
	}
}

void scr_char(char c){
	if(current_location >= 80*25*2)
	{
		scroll_down();
		current_location -= 80*2;
		vidptr[current_location] = c;
		vidptr[current_location+1] = 0x07;	
		current_location += 2;
		update_cursor(current_location/2);
	}
	else {
	vidptr[current_location] = c;
	vidptr[current_location+1] = 0x07;	
	current_location += 2;
	update_cursor(current_location/2);
	}
}

void update_cursor(unsigned short position){
	write_port(0x3D4, 0x0F);
	write_port(0x3D5, (unsigned char)(position&0xFF));

	write_port(0x3D4,0x0E);
	write_port(0x3D5, (unsigned char)((position>>8)&0xFF));
}

void scroll_down(void){
	int i = 0;
		for(i=0;i<80*25*2;i++){
		vidptr[i] = vidptr[i+80*2];
		}
}

void backspace(void){
	vidptr[current_location-2] = ' ';
	vidptr[current_location-1] = 0x07;
	current_location = current_location - 2;
	update_cursor(current_location/2);
}

void print_int(int a){
	char buffer[10];
	int i = 0;
	int digit = 0;
	while(a/10 != 0){
		digit = a % 10;
		a = a / 10;
		buffer[i] = '0' + digit;
		i++;
	}
	digit = a % 10;
	buffer[i] = '0' + digit;
	char finalbuffer[10];
	int j = 0;
	for(j = 0;j<(i+1);j++){
	finalbuffer[j] = buffer[i - j];
	}
	finalbuffer[j + 1] = '\0';
	scr_write(finalbuffer);
}

void change_position(int x,int y){
	current_location = 2*x + 80 * 2 * y;
}

void color_char(char c,unsigned char forecolour,unsigned char backcolour){
	u16int attrib = (backcolour << 4) | (forecolour & 0x0F);
	if(current_location >= 80*25*2)
	{
		scroll_down();
		current_location -= 80*2;
		vidptr[current_location] = c;
		vidptr[current_location+1] = attrib;	
		current_location += 2;
		update_cursor(current_location/2);
	}
	else {
	vidptr[current_location] = c;
	vidptr[current_location+1] = attrib;	
	current_location += 2;
	update_cursor(current_location/2);
	}
}

void print_hex_byte(unsigned char hex){
	unsigned char str[3];
	if((hex % 16) > 9){
	str[1] = '@' + (hex % 16 - 9);
	} else {
	str[1] = '0' + (hex % 16);
	}
	hex = hex / 16;
	if((hex % 16) > 9){
	str[0] = '@' + (hex % 16 - 9);
	} else {
	str[0] = '0' + (hex % 16);
	}
	str[2] = '\0';
	scr_write(str);
}

void hd_read_c(u8int sector, u16int cylinder,char buffer[512]){
	u8int require_servicing = 1;
	write_port(0x1F6,0x0A0);	//Drive and head
	write_port(0x1F2,1);		//How many sectors?
	write_port(0x1F3,sector);	//What sector?
	u8int cylinder_lo = cylinder & 0xff;
	write_port(0x1F4,cylinder_lo);	//Cylinder low
	u8int cylinder_hi = (cylinder >> 8) & 0xff;
	write_port(0x1F5,cylinder_hi);	//Cylinder high
	
	write_port(0x1F7,0x20);		//Send Read with RETRY command

	while(require_servicing == 1){
	u8int status = read_port(0x1F7);
		if(status & 8)
			require_servicing = 1;
		else
			require_servicing = 0;
	}

	int i = 0;
	for(i = 0;i < 511;i++){
	buffer[i] = read_port(0x1F0);
	}
}

int bin_to_dec(char bits[8]){
	int x = 0;
	x += bits[7];
	x += 2 * bits[6];
	x += power(2,2) * bits[5];
	x += power(2,3) * bits[4];
	x += power(2,4) * bits[3];
	x += power(2,5) * bits[2];
	x += power(2,6) * bits[1];
	x += power(2,7) * bits[0];
	return x;
}

int power(int a,int b){
	int i = 1;
	int result = 1;
	while(i<=b){
		result = result * a;
		i++;
	}
	return result;
}

void dec_to_bin(u8int dec,char bits[8]){
	int d[8];
	int i = 0;
	while(dec>0)
	{
		d[i] = dec % 2;
		i++;
		dec /= 2;
	}
	int j = i - 1;
	int counter = 0;
	for(j = i - 1;j>=0;j--){
		bits[counter] = d[j];
		counter++;
	}
}

void pokeb(u16int segment,u16int offset,u8int value){
	u32int address = segment * 0x100 + offset;
	u8int *memspace = (u8int*)address;
	*memspace = (u8int)value;
}

u8int peekb(u16int segment,u16int offset){
	u32int address = segment * 0x100 + offset;
	u8int *memspace = (u8int*)address;
	u8int value = *memspace;
	return value;
}
