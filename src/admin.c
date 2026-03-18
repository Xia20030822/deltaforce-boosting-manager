#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <unistd.h>
#include <sys/time.h>
#include "jpeg.h"
#include "lcd.h"
#include "slide.h"
#include "touch.h"
#include "showvideo.h"
#include "font.h"
#include "user.h"

unsigned long get_timestamp_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void show_admin_login_screen()
{
    usleep(5000);
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
    printf("test6\n");

    // 显示标题
    printf("test7\n");
    Display_characterX(250, 50, "管理员登录", 0xFF000000, 3);
    printf("test8\n");

    // 显示密码输入框背景
    Clean_Area(250, 150, 300, 60, 0xFFE0E0E0);

    // 显示确认键
    show_jpeg(555, 50, "confirm1.jpg");
    printf("F11confirm_test1\n");

    // 显示数字键盘
    show_number_keyboard();

    // 返回按钮
    show_jpeg(20, 50, "return1.jpg");
}

int is_admin_area(int x, int y)
{
    return (x >= 20 && x <= 380 && y >= 180 && y <= 300);
}

// 管理员登录处理
void admin_login_screen(int fd)
{

    printf("test4\n");
    TouchPoint tp;
    int password[13] = {0};
    int input_count = 0;
    char display_str[32];

    show_admin_login_screen();

    while (1)
    {
        if (touch_read(fd, &tp) > 0)
        {
            unsigned long now = get_timestamp_ms();

            // 边缘检测 + 200ms防抖
            if (tp.pressed > 0 && last_pressed == 0 &&
                (now - last_click_time) > 200)
            {
                last_click_time = now;
                printf("点击: (%d, %d)\n", tp.x, tp.y);

                // 返回按钮区域
                if (tp.x > 20 && tp.x < 250 && tp.y > 50 && tp.y < 360)
                {
                    printf("返回\n");
                    return;
                }

                // 数字键盘区域
                int num = get_key_number(tp.x, tp.y);
                printf("数字: %d\n", num);

                if (num >= 0 && num <= 9)
                {
                    if (input_count < 13)
                    {
                        password[input_count] = num;
                        input_count++;

                        // 显示 *
                        for (int i = 0; i < input_count; i++)
                            display_str[i] = '*';
                        display_str[input_count] = '\0';

                        Clean_Area(260, 160, 280, 40, 0xFFE0E0E0);
                        Display_characterX(270, 165, display_str, 0xFF000000, 2);
                    }
                }

                // 确认按钮区域
                if (tp.y > 50 && tp.y < 300 && tp.x > 550 && tp.x < 750)
                {
                    printf("确认，验证密码\n");

                    // 验证密码 (123456)
                    int correct = 1;
                    for (int i = 0; i < 6; i++)
                    {
                        if (password[i] != admin_password[i])
                        {
                            correct = 0;
                            break;
                        }
                    }

                    if (correct && input_count == 6)
                    {
                        printf("密码正确，进入管理员后台\n");
                        Display_characterX(300, 250, "man!", 0xFF00AA00, 3);
                        sleep(1);
                        // 进入管理员后台
                        admin_main_screen(fd);
                        // 返回后重新显示登录界面
                        show_admin_login_screen();
                        input_count = 0;
                        memset(password, 0, sizeof(password));
                    }
                    else
                    {
                        Display_characterX(280, 250, "manbaout!", 0xFFFF0000, 3);
                        Clean_Area(260, 160, 280, 40, 0xFFE0E0E0);
                        sleep(1);
                        // 清空重新输入
                        input_count = 0;
                    }
                }
                else
                {
                    Clean_Area(280, 250, 280, 60, 0xFFFFFFFF);
                }
            }

            last_pressed = tp.pressed;
        }

        usleep(10000); // 10ms轮询
    }
}

void admin_main_screen(int fd)
{
    sleep(1);
    printf("test9\n");
    TouchPoint tp;

    // 清屏
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
    show_jpeg(20, 50, "return1.jpg");

    Display_characterX(200, 50, "管理员后台", 0xFF000000, 4);
    Display_characterX(200, 200, "查询日志文件", 0xFF000000, 3);
    Display_characterX(200, 280, "查询各类请求", 0xFF000000, 3);
    Display_characterX(200, 360, "查询公司总收益", 0xFF000000, 3);

    // 等待触摸返回
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

                if (tp.x > 0 && tp.x < 210 && tp.y > 50 && tp.y < 360)
                {
                    printf("返回\n");
                    return;
                }
                else if (tp.x > 200 && tp.x < 350 && tp.y > 200 && tp.y < 280)
                {
                    msg_show(fd, "log.txt");
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(20, 50, "return1.jpg");

                    Display_characterX(200, 50, "管理员后台", 0xFF000000, 4);
                    Display_characterX(200, 200, "查询日志文件", 0xFF000000, 3);
                    Display_characterX(200, 280, "查询各类请求", 0xFF000000, 3);
                    Display_characterX(200, 360, "查询公司总收益", 0xFF000000, 3);
                    /* 查询日志文件 */
                }
                else if (tp.x > 200 && tp.x < 350 && tp.y > 280 && tp.y < 360)
                {
                    request_read(fd);
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(20, 50, "return1.jpg");

                    Display_characterX(200, 50, "管理员后台", 0xFF000000, 4);
                    Display_characterX(200, 200, "查询日志文件", 0xFF000000, 3);
                    Display_characterX(200, 280, "查询各类请求", 0xFF000000, 3);
                    Display_characterX(200, 360, "查询公司总收益", 0xFF000000, 3);
                    /* 查询各类请求 */
                }
                else if (tp.x > 200 && tp.x < 350 && tp.y > 360 && tp.y < 430)
                {
                    msg_show(fd, "bill.txt");
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(20, 50, "return1.jpg");

                    Display_characterX(200, 50, "管理员后台", 0xFF000000, 4);
                    Display_characterX(200, 200, "查询日志文件", 0xFF000000, 3);
                    Display_characterX(200, 280, "查询各类请求", 0xFF000000, 3);
                    Display_characterX(200, 360, "查询公司总收益", 0xFF000000, 3);
                    /* 查询公司总资产 */
                }
                else if (tp.x > 200 && tp.x < 350 && tp.y > 430 && tp.y < 480)
                {
                    msg_show(fd, "boosting_data.txt");
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(20, 50, "return1.jpg");

                    Display_characterX(200, 50, "管理员后台", 0xFF000000, 4);
                    Display_characterX(200, 200, "查询日志文件", 0xFF000000, 3);
                    Display_characterX(200, 280, "查询各类请求", 0xFF000000, 3);
                    Display_characterX(200, 360, "查询公司总收益", 0xFF000000, 3);
                    /* 查询公司总资产 */
                }
            }
        }
    }
}
