#ifndef MEMSET_H
#define MEMSET_H

void *memset(void *s, char c, int n);
void scr_write(char *str);
void scr_clear(void);
void scr_newline(void);
void print_int(int a);
void backspace(void);
void change_position(int x,int y);
void color_char(char c,unsigned char forecolour,unsigned char backcolour);

#endif
