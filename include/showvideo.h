#ifndef SHOWVIDEO_H
#define SHOWVIDEO_H
#include <stdio.h>
#include <stdlib.h>

// 数字键盘区域定义
// 屏幕最下方，每个数字宽80，高160
// 总宽度 80*10 = 800，高度 160
#define KEYBOARD_Y 320 // 键盘起始Y坐标 (480-160=320)
#define KEY_W 80       // 每个数字键宽度
#define KEY_H 160      // 键盘高度
extern int admin_password[100];

void user_login_main(int fd);
void user_reg_screen(int fd);
void function(int fd);

int init_all();
void play_mp4(const char *filename, int fd);
void admin_login_screen(int fd);
int is_admin_area(int x, int y);

void admin_main_screen(int fd);
void user_login_screen(int fd);
int is_user_area(int x, int y);

#endif