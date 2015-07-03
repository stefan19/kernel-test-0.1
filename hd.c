#include "Include/types.h"
#include "Include/hd.h"
#include "Include/system.h"

u8int sector = 0;
u8int cylinder_lo = 0;
u8int cylinder_hi = 0;
u8int drive_head = 0;
u32int lba = 0;
u8int transfer_finished = 0;

void head_to_bin(u8int head2,char head_bits[4]);
void ToCHS(int lba,u8int head,u16int track,u8int sector,u16int sectors_per_track);

int first_root_dir_sector;
int first_data_sector;
int first_fat_sector;
u8int sectors_per_cluster = 0;
u32int volume_lba;

void read_sector(u8int sector2,u16int cylinder2,u8int head2,char read_buffer[512]){
	sector = sector2;
	
	cylinder_lo = cylinder2 & 0xFF;
	cylinder_hi = (cylinder2 >> 8) & 0xFF;
	
	char head_bits[4];
	head_to_bin((head2 & 0xF),head_bits);
	char bits[] = {1,0,1,0,head_bits[0],head_bits[1],head_bits[2],head_bits[3]};
	drive_head = bin_to_dec(bits);

	u8int *buffer = (u8int*) read_hd();
	buffer = (u8int*) read_hd();

	int i = 0;
	for(i=0;i<511;i++){
	read_buffer[i] = buffer[i];
	}
}

void read_sector_lba(u16int lba2,char read_buffer[512]){
	lba = lba2;
	u8int* buffer = (u8int*) read_hd_lba();
	buffer = (u8int*) read_hd_lba();

	int i = 0;
	for(i=0;i<511;i++){
	read_buffer[i] = buffer[i];
	}
}

void read_sectors_lba(u32int lba2,char read_buffer[],u8int sector_count2){
	write_port(0x01F6, ( ( (lba2 >> 24) & 0x0F)|0xE0) );
	write_port(0x01F2, 1);
	write_port(0x01F3, (lba2 & 0xFF));
	write_port(0x01F4, (lba2 >> 8) & 0xFF);
	write_port(0x01F5, (lba2 >> 16) & 0xFF);
	write_port(0x01F7, 0x20);
	int i = 0;
	u8int status = read_port(0x01F7);
	while( (status & 8) == 0){
		status = read_port(0x01F7);
	}
	for(i=0;i<256;i++){
		u16int tmpword = inw(0x01F0);
		read_buffer[i*2] = (u8int)(tmpword & 0xFF);
		read_buffer[i*2+1] = (u8int)((tmpword >> 8) & 0xFF);
	}
	return;	
}

void read_volume_info(void){
	char buffer[512];
	read_sector(1,0,0,buffer);
	volume_lba = buffer[0x1be + 11] * 0x1000000 + buffer[0x1be + 10] * 0x10000 + buffer[0x1be + 9] * 0x100 + buffer[0x1be + 8];

	read_sector_lba(volume_lba,buffer);
	
	u16int bytes_per_sector = buffer[12] * 0x100 + buffer[11];
	sectors_per_cluster = buffer[13];
	u16int reserved_sectors = buffer[15] * 0x100 + buffer[14];
	u8int fat_count = buffer[16];
	u16int directory_entries = buffer[18] * 0x100 + buffer[17];
	u16int total_sectors = buffer[20] * 0x100 + buffer[19];
	u8int sector_count1 = buffer[22];
	u8int sector_count2 = buffer[23];
	u16int sectors_per_fat = sector_count2 * 0x100 + sector_count1;
	u16int sectors_per_track = buffer[25] * 0x100 + buffer[24];

	int root_dir_sectors = ((directory_entries * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
	first_data_sector = reserved_sectors + (fat_count * sectors_per_fat) + root_dir_sectors;
	first_root_dir_sector = reserved_sectors + fat_count * sectors_per_fat;
	first_fat_sector = reserved_sectors;
}

void head_to_bin(u8int head2,char head_bits[4]){
	head_bits[3] = head2 % 2;
	head2 /= 2;
	head_bits[2] = head2 % 2;
	head2 /= 2;
	head_bits[1] = head2 % 2;
	head2 /= 2;
	head_bits[0] = head2 % 2;
}

void ToCHS(int lba,u8int head,u16int track,u8int sector,u16int sectors_per_track){
	head = (lba % (sectors_per_track * 2)) / sectors_per_track;
	track = (lba / (sectors_per_track * 2));
	sector = (lba % sectors_per_track + 1);
}

void dir(){
	char buffer[512];
	char name[20];
	read_sector_lba(first_root_dir_sector + volume_lba,buffer);
	int i = 0;
	int j = 0;
	char is_dir = 0;
	int char_counter = 0;
	for(i = 0;i<15;i++){
		is_dir = 0;
		if(buffer[i * 32] == 0)
			return;
		else if(buffer[i * 32] == 0xE5)
			return;
		else{
			if(buffer[i*32 + 11] == 0x10){
				is_dir = 1;}
			else{
				is_dir = 0;}
			for(j = 0;j<11;j++){
				if((j == 8) && (is_dir == 0)){
				name[j] = '.';
				name[j+1] = buffer[i*32 + 8];
				name[j+2] = buffer[i*32 + 9];
				name[j+3] = buffer[i*32 + 10];
				break;
				} else {
				name[j] = buffer[i*32 + j];
				}
			}
		}
		if(is_dir == 0){
			name[12] = '\0';
		}
		else if(is_dir == 1){
			name[11] = ' ';
			name[12] = ' ';
			name[13] = '<';
			name[14] = 'D';
			name[15] = 'I';
			name[16] = 'R';
			name[17] = '>';
			name[18] = '\0';
		}
			
		scr_write(name);
		scr_newline();
	}
}

void read_file(char filename[11]){
	read_volume_info();
	char buffer[512];
	read_sector_lba(first_root_dir_sector + volume_lba,buffer);
	int i = 0;
	char names_same = 1;
	int entry_count = 0;
	char file_found = 0;
	while(buffer[i*32] != 0x00){
		names_same = 1;
		int j = 0;
		for(j=0;j<11;j++){
			if(buffer[i*32 + j] != filename[j])
				names_same = 0;
		}
		if(names_same == 1){
			file_found = 1;
			entry_count = i;
			break;
		}
		i++;
	}

	if(file_found == 1){
		u16int first_file_cluster = buffer[entry_count*32 + 27] * 0x100 + buffer[entry_count*32 + 26];
		u32int file_size = buffer[entry_count*32 + 31] * 0x1000000 + buffer[entry_count*32 + 30] * 0x10000 + buffer[entry_count*32 + 29] * 0x100 + buffer[entry_count*32 + 28];
		u32int first_file_sector = (first_file_cluster - 2) * sectors_per_cluster + first_data_sector + volume_lba;

		char cluster_buffer[512*8];
		int counter = 0;
		for(counter=0;counter<8;counter++){
			u8int sect_buffer[512];
			read_sectors_lba(first_file_sector + counter,sect_buffer,1);
			i = 0;
			for(i=0;i<512;i++){
			cluster_buffer[counter*512+i] = sect_buffer[i]; 
			}	
		}
	
		i = 0;
		while(i < file_size){
			if( (cluster_buffer[i] == 0x0D) && (cluster_buffer[i+1] == 0x0A)){
				scr_newline();
				i += 2;
			} else {
				scr_char(cluster_buffer[i]);
				i++;
			}
		}
		
		/*while(1){
			u8int FAT_table[512];
			u32int fat_offset = first_file_cluster * 2;
			u32int fat_sector = first_fat_sector + (fat_offset / 512);
			u32int ent_offset = fat_offset % 512;

			read_sector_lba(fat_sector + volume_lba,FAT_table);
		
			u16int table_value = FAT_table[ent_offset+1] * 0x100 + FAT_table[ent_offset];

			if(table_value >= 0xFFF8){
				break;
			}
			else if(table_value == 0xFFF7){
				
			}
			else{
				u32int file_sector = (table_value - 2) * sectors_per_cluster + first_data_sector + volume_lba;
				i = 0;
				for(i=0;i<sectors_per_cluster;i += 8){
					char sectors_buffer[512*8];
					read_sectors_lba(first_file_sector + i,sectors_buffer,8);
					int j = 0;
					for(j=0;j<512*8;j++){
						cluster_buffer[j + 512*i] = sectors_buffer[j];
					}
				}
	
				i = 0;
				while(i < file_size){
					if( (cluster_buffer[i] == 0x0D) && (cluster_buffer[i+1] == 0x0A)){
						scr_newline();
						i += 2;
					} else {
						scr_char(cluster_buffer[i]);
						i++;
					}
				}
			}
			first_file_cluster += 1;
		} */
	}
	else{
		scr_write("No such file or directory");
	}
}

void dma_read(u32int io_port){
		u8int buffer[512];
		u32int memspace = (u32int)&buffer;
		struct prdt_struct{
			u32int buffer_addr;
			u16int count;
			u16int msb;
		}__attribute__((packed));
		struct prdt_struct prdt;
		prdt.buffer_addr = memspace;
		prdt.count = 512;
		prdt.msb = 0x8000;
		
		//write_port(io_port+4,((u32int)&prdt >> 24)&0xFF);
		//write_port(io_port+5,((u32int)&prdt >> 16)&0xFF);
		//write_port(io_port+6,((u32int)&prdt >> 8)&0xFF);
		//write_port(io_port+7,((u32int)&prdt)&0xFF);
		outl(io_port+4,(u32int)&prdt);
		write_port(io_port+2,0x06);//6 is 110 in binary -> Clear interrupt bit and err bit.

		u32int lba2 = 0;		//Send lba sector number and count
		write_port(0x01F6, ( ( (lba2 >> 24) & 0x0F)|0xE0) );
		write_port(0x01F2, 1);
		write_port(0x01F3, (lba2 & 0xFF));
		write_port(0x01F4, (lba2 >> 8) & 0xFF);
		write_port(0x01F5, (lba2 >> 16) & 0xFF);
		write_port(0x01F7, 0xC8);	//DMA Read Command

		write_port(io_port,0x09);	//Start transfer between Disk Controller and memory

		u8int status = read_port(io_port + 2);
		if((status & 2) == 2){
			scr_newline();
			scr_write("FATAL: PRDT Error!");
		}
		else{
			scr_write("Transfer was successfully!");
			int i=0;
			for(i=0;i<16;i++){
			print_hex_byte(buffer[i]);	
			scr_char(' ');} 
			print_hex_byte(buffer[510]);
		}
		transfer_finished = 0;
}

void ide_handler_main(void){
	write_port(0x20,0x20);
	write_port(0xA0,0x20);
	transfer_finished = 1;
}

void ide_enable(void){
	write_port(0x21,0xF9);
	write_port(0xA1,0);
}
