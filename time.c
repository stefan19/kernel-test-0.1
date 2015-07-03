#include "Include/time.h"
#include "Include/system.h"

unsigned char second;
unsigned char minute;
unsigned char hour;

extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);

enum{
	cmos_address = 0x70,
	cmos_data = 0x71
};

int get_update_in_progress_flag(){
	write_port(cmos_address, 0x0A);
	return (read_port(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
	write_port(cmos_address, reg);
	return read_port(cmos_data);
}

void read_rtc(void){
	unsigned char last_second;
	unsigned char last_minute;
	unsigned char last_hour;
	unsigned char registerB;

	while (get_update_in_progress_flag());
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	
	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;

		while (get_update_in_progress_flag());
		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
	} while ( (last_second != second) || (last_minute != minute) || (last_hour != hour));

	registerB = get_RTC_register(0x0B);

	if(!(registerB & 0x04)){
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
	}

	if(!(registerB & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7F) + 12) % 24;
	}

	print_time(hour);
	scr_write(":");
	print_time(minute);
	scr_write(":");
	print_time(second);
}

int print_time(int n){
	char str[3];
	if(n / 10 == 0){
	str[0] = '0';
	str[1] = '0' + n;
	} else {
	str[1] = '0' + n%10;
	n = n / 10;
	str[0] = '0' + n%10;
	}
	str[2] = '\0';
	scr_write(str);
}
