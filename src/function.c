#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include </home/a/arm-gif/include/gif_lib.h>
#include "jpeg.h"
#include "lcd.h"
#include "slide.h"
#include "touch.h"
#include "showvideo.h"
#include "font.h"
#include "user.h"
#include "adm.h"

void function(int fd)
{
    // int img_w, img_h;

    // struct fb_var_screeninfo vinfo;
    // ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

    // int bytes = vinfo.bits_per_pixel / 8;
    // int screensize =
    //     vinfo.xres * vinfo.yres * bytes;

    // unsigned char *fbp = mmap(NULL,
    //                           screensize,
    //                           PROT_READ | PROT_WRITE,
    //                           MAP_SHARED,
    //                           fd,
    //                           0);
    // show_gif("2.gif", fbp, &vinfo);
    show_jpeg(0, 0, "fengmian.jpg");

    TouchPoint tp;
    // 显示管理员界面与用户界面
    show_jpeg(20, 180, "admlogin.jpg");
    show_jpeg(420, 180, "userlogin.jpg");
    show_jpeg(325, 50, "boostinglogin.jpg");
    fout_show("欢迎光临黑曼巴肘击俱乐部", 0, 360, 4, 0xFFFF0000);

    while (1)
    {
        if (touch_read(fd, &tp) > 0)
        {
            unsigned long now = get_timestamp_ms();
            if (tp.pressed > 0 && last_pressed == 0 &&
                (now - last_click_time) > 200) // 检测
            {
                last_click_time = now;
                printf("clike1: (%d, %d)\n", tp.x, tp.y);

                // 检查是否点击管理员区域
                if (is_admin_area(tp.x, tp.y))
                {
                    printf("进入管理员登录\n");
                    admin_login_screen(fd);
                    sleep(1);
                    return; // 登录完成后返回
                }

                // 用户区域的判断
                printf("user_logintest\n");
                if (is_user_area(tp.x, tp.y))
                {
                    printf("进入用户登录\n");
                    user_login_screen(fd);
                    sleep(1);
                    return; // 登录完成后返回
                }
            }
        }
    }
}

int get_key_number(int x, int y)
{
    // 检查是否在键盘区域内
    if (y < KEYBOARD_Y || y >= KEYBOARD_Y + KEY_H)
    {
        return -1; // 不在键盘区域
    }

    // 检查X坐标范围 (0-800)
    if (x < 0 || x >= 800)
    {
        return -1;
    }

    // 计算数字 (0-9)
    int key = x / KEY_W;
    if (key == 9)
    {
        return 0; // 第10个位置是0
    }
    else
    {
        return key + 1; // 0-8 对应 1-9
    }
}

int get_key_number1(int x, int y)
{
    // 检查是否在键盘区域内
    if (y > 200)
    {
        return -1; // 不在键盘区域
    }

    // 检查X坐标范围 (0-800)
    if (x < 0 || x > 800)
    {
        return -1;
    }

    // 计算数字 (0-9)
    int key = x / KEY_W;
    if (key == 9)
    {
        return 0; // 第10个位置是0
    }
    else
    {
        return key + 1; // 0-8 对应 1-9
    }
}

// 显示数字键盘
void show_number_keyboard()
{
    // 显示键盘图片
    show_jpeg(0, KEYBOARD_Y, "keyboard.jpg");
}

// static int array_to_int(int *arr, int len)
// {
//     int result = 0;
//     int i;

//     for (i = 0; i < len; i++) {
//         result = result * 10 + arr[i];
//     }

//     return result;
// }

/**
 * 扣费并记录账单
 *
 * 参数:
 *   account     - 账号数组（如 {2, 4} 表示账号 24）
 *   account_len - 账号数组长度
 *   charge      - 要扣除的费用（如 100）
 *
 * 返回:
 *   0  - 成功扣费并记录
 *   -1 - 文件打开失败
 *   -2 - 未找到该账号
 *   -3 - 余额不足（扣费后小于0）
 *   -4 - 临时文件操作失败
 */
int process_charge(char *number, char *charge)
{
    FILE *fp;
    FILE *temp_fp;
    FILE *bill_fp;
    FILE *log_fp;
    char line[512];
    char line2[512];
    int found = 0;
    int i;
    int len;
    int current_revenue = 0;
    int new_revenue = 0;
    int final_remaining = 0;
    int charge_num;

    /* 将 charge 字符串转换为整数 */
    charge_num = atoi(charge);

    /* 打开 user_data.txt */
    fp = fopen("user_data.txt", "r");
    if (fp == NULL)
    {
        return -1;
    }

    /* 打开临时文件 */
    temp_fp = fopen("user_data_temp.txt", "w");
    if (temp_fp == NULL)
    {
        fclose(fp);
        return -4;
    }

    /* 逐行读取处理 user_data.txt */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int file_account = 0;
        int file_recharge = 0;
        int file_password = 0;
        int new_recharge = 0;
        char *p;
        char *password_start;
        char *recharge_start;
        char account_str[32];
        int is_target = 0;

        /* 去除换行符 */
        len = strlen(line);
        for (i = 0; i < len; i++)
        {
            if (line[i] == '\n' || line[i] == '\r')
            {
                line[i] = '\0';
                break;
            }
        }

        /* 跳过空行 */
        if (strlen(line) == 0)
        {
            fprintf(temp_fp, "\n");
            continue;
        }

        /* 解析账号 */
        p = line;
        if (strncmp(p, "account:", 8) == 0)
        {
            p = p + 8;
            /* 提取账号字符串 */
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 31)
            {
                account_str[i] = *p;
                i++;
                p++;
            }
            account_str[i] = '\0';

            /* 比较账号 */
            if (strcmp(account_str, number) == 0)
            {
                is_target = 1;
                found = 1;
            }

            /* 转换账号为整数 */
            file_account = atoi(account_str);
        }

        /* 检查是否目标账号 */
        if (is_target)
        {
            /* 找到 password */
            password_start = strstr(line, "password:");
            if (password_start != NULL)
            {
                password_start = password_start + 9;
                file_password = 0;
                while (*password_start >= '0' && *password_start <= '9')
                {
                    file_password = file_password * 10 + (*password_start - '0');
                    password_start++;
                }
            }

            /* 找到 recharge */
            recharge_start = strstr(line, "recharge:");
            if (recharge_start != NULL)
            {
                recharge_start = recharge_start + 9;
                file_recharge = 0;
                while (*recharge_start >= '0' && *recharge_start <= '9')
                {
                    file_recharge = file_recharge * 10 + (*recharge_start - '0');
                    recharge_start++;
                }
            }

            /* 计算新余额（扣除费用） */
            new_recharge = file_recharge - charge_num;

            /* 检查余额是否为0 */
            if (new_recharge == 0)
            {
                fclose(fp);
                fclose(temp_fp);
                remove("user_data_temp.txt");
                return 0;
            }

            /* 检查余额不足 */
            if (new_recharge < 0)
            {
                fclose(fp);
                fclose(temp_fp);
                remove("user_data_temp.txt");
                return -3;
            }

            final_remaining = new_recharge;

            /* 构造新行 */
            fprintf(temp_fp, "account:%s,password:%d,recharge:%d\n",
                    number, file_password, new_recharge);
        }
        else
        {
            /* 非目标账号，原样写入 */
            fprintf(temp_fp, "%s\n", line);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    /* 检查是否找到账号 */
    if (!found)
    {
        remove("user_data_temp.txt");
        return -2;
    }

    /* 替换原文件 */
    remove("user_data.txt");
    rename("user_data_temp.txt", "user_data.txt");

    /* 更新 bill.txt 的 total_revenue */
    bill_fp = fopen("bill.txt", "r");
    if (bill_fp != NULL)
    {
        if (fgets(line2, sizeof(line2), bill_fp) != NULL)
        {
            char *p2;
            /* 解析 total_revenue */
            p2 = strstr(line2, "total_revenue:");
            if (p2 != NULL)
            {
                p2 = p2 + 14;
                current_revenue = 0;
                while (*p2 >= '0' && *p2 <= '9')
                {
                    current_revenue = current_revenue * 10 + (*p2 - '0');
                    p2++;
                }
            }
        }
        fclose(bill_fp);
    }

    /* 计算新的总收入 */
    new_revenue = current_revenue + charge_num;

    /* 写回 bill.txt */
    bill_fp = fopen("bill.txt", "w");
    if (bill_fp != NULL)
    {
        fprintf(bill_fp, "total_revenue:%d\n", new_revenue);
        fclose(bill_fp);
    }

    /* 添加日志到 log.txt */
    log_fp = fopen("log.txt", "a");
    if (log_fp == NULL)
    {
        log_fp = fopen("log.txt", "w");
    }
    if (log_fp != NULL)
    {
        fprintf(log_fp, "account:%s,charge:%s,remaining:%d,total_revenue:%d\n",
                number, charge, final_remaining, new_revenue);
        fclose(log_fp);
    }

    return 0;
}