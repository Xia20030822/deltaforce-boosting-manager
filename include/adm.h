#ifndef __ADM_H__
#define __ADM_H__

void show_admin_login_screen();
int is_admin_area(int x, int y);
void admin_login_screen(int fd);
void admin_main_screen(int fd);
unsigned long get_timestamp_ms();

#endif