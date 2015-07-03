#ifndef HD_H
#define HD_H

void read_sector(u8int sector2,u16int cylinder,u8int head2,char buffer[512]);
void read_sector_lba(u16int lba,char read_buffer[512]);
#endif
