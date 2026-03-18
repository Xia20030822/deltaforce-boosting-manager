#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <linux/input.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "jpeg.h"
#include "lcd.h"
#include "slide.h"
#include "touch.h"
#include "font.h"
#include "user.h"
#include "adm.h"
#include "boosting.h"
#include "boosting_read.h"

static BoostingRecord records[MAX_RECORDS];
static int record_count = 0;
static int data_loaded = 0;

int verify_boosting(int *account, int account_len)
{
    FILE *fp = fopen("boosting_data.txt", "r");
    if (fp == NULL)
    {
        printf("notfinddata.txt\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // 解析文件中的账号和密码
        int file_account[24] = {0};

        int file_account_len = 0;

        char *p = line;

        // 跳过 "number:" 前缀
        if (strncmp(p, "number:", 7) != 0)
            continue;
        p += 7;

        // 读取账号数字
        while (*p >= '0' && *p <= '9' && file_account_len < 24)
        {
            file_account[file_account_len++] = *p - '0';
            p++;
        }
        printf("readnumber\n");
        // 跳过 ",password:" 前缀
        if (strncmp(p, ",price:", 7) != 0) // 孩子们，gb2312每个汉字字节不一样,gb2312不要用strncmp！
            continue;
        p += 7;

        // 比较账号
        if (account_len != file_account_len)
            continue;
        int account_match = 1;
        for (int i = 0; i < account_len; i++)
        {
            if (account[i] != file_account[i])
            {
                account_match = 0;
                break;
            }
        }
        printf("compareaccount\n");

        if (account_match)
        {
            fclose(fp);
            return 1; // 验证成功
        }
    }

    fclose(fp);
    return 0; // 验证失败
}

void boosting_screen(int fd, char *number, int *account, int account_len)
{
    TouchPoint tp;
    int i = 0;
    char price[64], level[16], photo[128], gif[128], name[128];
    int time;
    char boosting_name[256], boosting_lv[256], boosting_price[256], boosting_time[256];
    int recharge1;

    static int last_pressed = 1;

    // 清屏
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
    show_jpeg(0, 0, "boosting_screen.jpg");
    show_jpeg(50, 400, "return3.jpg");
    show_jpeg(400, 350, "confirm2.jpg");
    for (i = 0; number[i] != '\0'; i++)
    {
        printf("%c\n", number[i]);
    }

    if (!get_boosting_by_number(number, price, level, photo, gif, name, &time))
    {
        show_jpeg(50, 90, photo);
        // 打印护航姓名
        snprintf(boosting_name, 666, "护航姓名:%s", name);
        Display_characterX(355, 90, boosting_name, 0xFFFF0000, 2);
        // 打印护航等级
        snprintf(boosting_lv, 666, "护航等级:%s", level);
        Display_characterX(355, 150, boosting_lv, 0xFFFF0000, 2);
        // 打印护航价位
        snprintf(boosting_price, 666, "护航价位:%s", price);
        Display_characterX(355, 210, boosting_price, 0xFFFF0000, 2);
        // 打印是否空闲
        if (time == 1)
        {
            show_jpeg(400, 350, "busy.jpg");
            snprintf(boosting_time, 666, "是否空闲:%s", "忙碌");
            Display_characterX(355, 270, boosting_time, 0xFFFF0000, 2);
        }
        else if (time == 0)
        {
            snprintf(boosting_time, 666, "是否空闲:%s", "空闲");
            Display_characterX(355, 270, boosting_time, 0xFFFF0000, 2);
        }
    }
    else
    {
        printf("打开文件失败\n");
    }
    // 主循环
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

                if (tp.x > 40 && tp.x < 120 && tp.y > 360 && tp.y < 480)
                {
                    printf("返回\n");
                    return;
                }
                else if (tp.x > 390 && tp.x < 480 && tp.y > 340 && tp.y < 420)
                {
                    // 点单函数
                    process_charge(number, price);
                    request_call(account, account_len, 2);
                    time = 1;
                    show_jpeg(325, 200, "bill_success.jpg");
                    sleep(1);
                    show_jpeg(0, 0, "boosting_screen.jpg");
                    show_jpeg(50, 400, "return3.jpg");
                    show_jpeg(400, 350, "confirm2.jpg");
                    show_jpeg(50, 90, photo);
                    // 打印护航姓名
                    snprintf(boosting_name, 666, "护航姓名:%s", name);
                    Display_characterX(355, 90, boosting_name, 0xFFFF0000, 2);
                    // 打印护航等级
                    snprintf(boosting_lv, 666, "护航等级:%s", level);
                    Display_characterX(355, 150, boosting_lv, 0xFFFF0000, 2);
                    // 打印护航价位
                    snprintf(boosting_price, 666, "护航价位:%s", price);
                    Display_characterX(355, 210, boosting_price, 0xFFFF0000, 2);
                    // 打印是否空闲
                    if (time == 1)
                    {
                        show_jpeg(400, 350, "busy.jpg");
                        snprintf(boosting_time, 666, "是否空闲:%s", "忙碌");
                        Display_characterX(355, 270, boosting_time, 0xFFFF0000, 2);
                    }
                    else if (time == 0)
                    {
                        snprintf(boosting_time, 666, "是否空闲:%s", "空闲");
                        Display_characterX(355, 270, boosting_time, 0xFFFF0000, 2);
                    }
                }
            }
        }
        last_pressed = tp.pressed;
    }
}
