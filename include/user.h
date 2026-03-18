#ifndef USER_H
#define USER_H
#include <linux/fb.h>
#include "touch.h"
void user_login_main(int fd);
void user_login_screen(int fd);
void user_reg_screen(int fd);
void user_main_screen(int fd, int *account, int account_len);
void user_recharge(int fd, int *account, int account_len);
void user_select(int fd, int *account, int account_len);
void user_call(int fd, int *account, int account_len);
void request_call(int *account, int account_len, int n);
void request_read(int fd);
void msg_show(int fd, char *filename);
void show_gif(const char *filename,
              unsigned char *fbp,
              struct fb_var_screeninfo *vinfo);

int is_user_area(int x, int y);
int input_key_password(TouchPoint tp, int fd, int x, int y, int *password);
int verify_account(int *account, int account_len, int *password, int password_len);
int save_account_to_file(int *account, int account_len, int *password, int password_len);
int show_all_accounts(void);
void show_all_boosters(int fd, int *account, int account_len);
int array_to_int(int *arr, int len);
int input_key_account(TouchPoint tp, int fd, int x, int y, int *password);

// 钱相关:
int modify_recharge_by_account(int tp, int recharge, int *account, int account_len);
int process_charge(char *number, char *charge);
struct user_data
{
    int user_account[100];
    int user_password[100];
    int Balance;
} user;

#endif