#ifndef LCD_H
#define LCD_H

extern unsigned int *fbp;
extern int screen_width;
extern int screen_height;
extern long screensize;
int get_key_number1(int x, int y);
void init_fb();
int get_key_number(int x, int y);
void show_number_keyboard();

#endif