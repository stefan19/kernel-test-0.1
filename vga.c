#include "Include/types.h"

#define VGA_AC_INDEX		0x3C0
#define VGA_AC_WRITE		0x3C0
#define VGA_AC_READ		0x3C1
#define VGA_MISC_WRITE		0x3C2
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5
#define VGA_DAC_READ_INDEX	0x3C7
#define VGA_DAC_WRITE_INDEX	0x3C8
#define VGA_DAC_DATA		0x3C9
#define VGA_MISC_READ		0x3CC
#define VGA_GC_INDEX		0x3CE
#define VGA_GC_DATA		0x3CF

#define VGA_CRTC_INDEX		0x3D4
#define VGA_CRTC_DATA		0x3D5
#define VGA_INSTAT_READ		0x3DA

#define VGA_NUM_SEQ_REGS	5
#define VGA_NUM_CRTC_REGS	25
#define VGA_NUM_GC_REGS		9
#define VGA_NUM_AC_REGS		21

unsigned char g_320x200x256[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

void set_video_mode(u8int *regs){
	u8int i;

/* write MICELLANEOUS reg */
	write_port(VGA_MISC_WRITE, *regs);
	regs++;

/* write SEQUENCER regs */
	for(i = 0;i<VGA_NUM_SEQ_REGS;i++)
	{
		write_port(VGA_SEQ_INDEX, i);
		write_port(VGA_SEQ_DATA, *regs);
		regs++;
	}

/* Unlock CRTC Registers */
	write_port(VGA_CRTC_INDEX, 0x03);
	write_port(VGA_CRTC_DATA, read_port(VGA_CRTC_DATA) | 0x80);
	write_port(VGA_CRTC_INDEX, 0x11);
	write_port(VGA_CRTC_DATA, read_port(VGA_CRTC_DATA) & ~0x80);

	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

/* write CRTC regs */
	for(i=0;i<VGA_NUM_CRTC_REGS;i++){
		write_port(VGA_CRTC_INDEX, i);
		write_port(VGA_CRTC_DATA, *regs);
		regs++;
	}

/* write GRAPHICS CONTROLLER regs */
	for(i=0;i<VGA_NUM_GC_REGS;i++){
		write_port(VGA_GC_INDEX, i);
		write_port(VGA_GC_DATA, *regs);
		regs++;
	}

/* write ATTRIBUTE CONTROLLER regs */
	for(i=0;i<VGA_NUM_AC_REGS;i++){
		(void)read_port(VGA_INSTAT_READ);
		write_port(VGA_AC_INDEX, i);
		write_port(VGA_AC_WRITE, *regs);
		regs++;
	}

/* lock 16-color pallete and unblank display */
	(void)read_port(VGA_INSTAT_READ);
	write_port(VGA_AC_INDEX, 0x20);	
}

static unsigned get_fb_seg(){
	unsigned seg;

	write_port(VGA_GC_INDEX,6);
	seg = read_port(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}

void write_pixel8(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;
	
	wd_in_bytes = 320;
	off = wd_in_bytes * y + x;
	char* vgamemory = (char*)0xA0000;
	vgamemory[off] = c;
}

void init_screen(void){
	set_video_mode(g_320x200x256);
}

void clear_screen(char c){
	int x = 0,y = 0;
		for(y=0;y<200;y++){
			for(x=0;x<320;x++){
			write_pixel8(x,y,c);
			}
		}
}

void draw_rectangle(int startx,int starty,int endx,int endy,char c){
	int x = 0,y=0;
	for(y=starty;y<=endy;y++){
		for(x=startx;x<=endx;x++){
		write_pixel8(x,y,c);
		}
	}
}

char A_array[8] = {0x18,0x3C,0x42,0x3C,0x42,0x42,0x00,0x00};

void draw_char(int startx,int starty){
	int x = 0,y = 0;
	for(y=starty;y<(starty + 8);y++){
		for(x=startx+7;x>startx;x--){
			char current_pixel = A_array[y - starty] & (1 << (x-startx));
			if(current_pixel != 0){
				write_pixel8(x,y,15);
			}
		}
	}
}
