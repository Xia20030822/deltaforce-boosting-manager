// 孩子们，管理员登录c文件和function放一块了
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

void msg_show(int fd, char *filename)
{
    // 读取文件内容
    TouchPoint tp;
    Clean_Area(0, 0, 800, 480, 0x000000);
    show_jpeg(50, 400, "return3.jpg");

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {

        return;
    }

    // 逐行读取并显示，每行y轴+60
    char line[256];
    int x = 10;
    int y = 10;
    unsigned int color = 0xFF0000;
    int size = 1;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // 去掉末尾换行符
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        // 显示当前行
        Display_characterX(x, y, (unsigned char *)line, color, size);

        // y轴+60换行
        y += 40;
    }

    fclose(fp);

    // 显示信息

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
            }
        }
    }
}

void request_read(int fd)
{
    TouchPoint tp;
    Clean_Area(0, 0, 800, 480, 0x000000);
    show_jpeg(50, 400, "return3.jpg");
    // 读取 request.txt 并解析
    FILE *fp = fopen("request.txt", "r");
    char line[256];
    if (fp != NULL)
    {

        if (fgets(line, sizeof(line), fp) != NULL)
        {
            // 解析 name
            int name = 0;
            // 解析 request_time
            char request_time[32] = {0};
            // 解析动作
            char action[32] = {0};

            sscanf(line, "name:%d,request_time:%19[^,],%31[^\n]",
                   &name, request_time, action);

            // 下方用于使用解析后的数据
            // void.....
        }
        fclose(fp);

        // 显示画面
        FILE *fp = fopen("request.txt", "r");
        if (fp == NULL)
        {

            return;
        }

        // 逐行读取并显示，每行y轴+60
        char line[256];
        int x = 10;
        int y = 10;
        unsigned int color = 0xFF0000;
        int size = 1;

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            // 去掉末尾换行符
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n')
            {
                line[len - 1] = '\0';
            }

            // 显示当前行
            Display_characterX(x, y, (unsigned char *)line, color, size);

            // y轴+60换行
            y += 40;
        }

        fclose(fp);

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
                }
            }
        }
    }
}

/// @brief
/// @param account
/// @param account_len
/// @param n n=0，记录充值日志，n=1，记录客服呼叫日志，n=2，记录点单日志
void request_call(int *account, int account_len, int n)
{
    // 获取系统时间
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    char time_str[64];
    int number = array_to_int(account, account_len);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    // 写入文件
    FILE *fp = fopen("request.txt", "w");
    if (fp != NULL && n == 1)
    {
        fprintf(fp, "name:%d,request_time:%s,呼叫客服\n", number, time_str);
        fclose(fp);
    }
    else if (fp != NULL && n == 0)
    {
        fprintf(fp, "name:%d,request_time:%s,充值\n", number, time_str);
        fclose(fp);
    }
    else if (fp != NULL && n == 2)
    {
        fprintf(fp, "name:%d,request_time:%s,呼叫打手\n", number, time_str);
        fclose(fp);
    }
}

void user_login_main(int fd)
{
    TouchPoint tp;
    int account[24] = {0};  // 存储账号
    int password[24] = {0}; // 存储密码
    int account_len = 0;    // 账号位数
    int password_len = 0;   // 密码位数
    int input_mode = 0;     // 0:无输入, 1:输入账号, 2:输入密码

    // 清屏
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);

    // 标题
    Display_characterX(300, 20, "用户登录", 0xFF000000, 3);

    // 账号输入框标签
    Display_characterX(150, 140, "账号:", 0xFF000000, 2);
    // 账号输入框背景
    Clean_Area(250, 130, 300, 50, 0xFFE0E0E0);

    // 密码输入框标签
    Display_characterX(150, 210, "密码:", 0xFF000000, 2);
    // 密码输入框背景
    Clean_Area(250, 200, 300, 50, 0xFFE0E0E0);

    // 确认注册按钮
    Clean_Area(580, 160, 120, 60, 0xFF00AA00);
    Display_characterX(595, 175, "确认", 0xFFFFFFFF, 2);

    // 返回按钮（底部）
    show_jpeg(-20, 50, "return1.jpg");

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
                printf("注册页面点击: (%d, %d)\n", tp.x, tp.y);

                // 账号输入框区域
                if (tp.x >= 250 && tp.x <= 550 && tp.y >= 130 && tp.y <= 200)
                {
                    printf("点击账号输入框1\n");
                    show_number_keyboard();

                    input_mode = 1;

                    // 调用输入函数
                    account_len = input_key_password(tp, fd, 250, 130, account);
                    printf("账号输入完成，共%d位\n", account_len);
                    input_mode = 0;
                }

                // 密码输入框区域
                else if (tp.x >= 250 && tp.x <= 550 && tp.y >= 200 && tp.y <= 250)
                {
                    printf("点击密码输入框\n");
                    show_number_keyboard();
                    input_mode = 2;

                    // 调用输入函数
                    password_len = input_key_password(tp, fd, 250, 200, password);
                    printf("密码输入完成，共%d位\n", password_len);
                    input_mode = 0;
                }

                // 确认按钮区域
                else if (tp.x >= 580 && tp.x <= 700 && tp.y >= 160 && tp.y <= 220)
                {
                    printf("点击确认登录\n");

                    // 检查是否已输入账号和密码
                    if (account_len > 0 && password_len > 0)
                    {
                        printf("passwordvity1\n");
                        if (verify_account(account, account_len, password, password_len))
                        {
                            printf("passwordvity2\n");
                            user_main_screen(fd, account, account_len);
                            // 清屏
                            Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);

                            // 标题
                            Display_characterX(300, 20, "用户登录", 0xFF000000, 3);

                            // 账号输入框标签
                            Display_characterX(150, 140, "账号:", 0xFF000000, 2);
                            // 账号输入框背景
                            Clean_Area(250, 130, 300, 50, 0xFFE0E0E0);

                            // 密码输入框标签
                            Display_characterX(150, 210, "密码:", 0xFF000000, 2);
                            // 密码输入框背景
                            Clean_Area(250, 200, 300, 50, 0xFFE0E0E0);

                            // 确认注册按钮
                            Clean_Area(580, 160, 120, 60, 0xFF00AA00);
                            Display_characterX(595, 175, "确认", 0xFFFFFFFF, 2);

                            // 返回按钮（底部）
                            show_jpeg(-20, 50, "return1.jpg");
                        }
                        else
                        {
                            Display_characterX(280, 250, "manbaout!", 0xFFFF0000, 3);
                            Clean_Area(250, 200, 300, 50, 0xFFE0E0E0);
                            Clean_Area(250, 130, 300, 50, 0xFFE0E0E0);
                        }
                    }
                    else
                    {
                        Display_characterX(200, 300, "请先输入账号和密码", 0xFFFF0000, 2);
                        sleep(1);
                        Clean_Area(200, 300, 400, 40, 0xFFFFFFFF);
                    }
                }

                // 返回按钮区域
                else if (tp.x > 0 && tp.x < 210 && tp.y > 50 && tp.y < 360)
                {
                    printf("返回\n");
                    return;
                }

                else if (input_mode == 0)
                {
                    // 点其他区域且不在输入模式时，清理键盘
                    Clean_Area(0, 320, 800, 160, 0xFFFFFFFF);
                }
            }

            last_pressed = tp.pressed;
        }

        usleep(10000);
    }
}

void user_main_screen(int fd, int *account, int account_len)
{
    printf("进入用户界面\n");
    TouchPoint tp;
    static int last_pressed = 1; // 记录上次状态（必须是static或全局）
    // 打印欢迎信息
    char account_str[100] = {0}; // 最多13位 + '\0'
    for (int i = 0; i < account_len && i < 13; i++)
    {
        account_str[i] = '0' + account[i]; // 数字转字符
    }
    char welcome_msg[100];
    snprintf(welcome_msg, 100, "尊敬的用户：%s你好", account_str);

    // 初始化显示
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
    show_jpeg(40, 100, "recharge1.jpg");
    show_jpeg(290, 100, "boosting.jpg");
    show_jpeg(540, 100, "call.jpg");
    show_jpeg(0, 400, "return2.jpg");
    Display_characterX(40, 20, welcome_msg, 0xFF000000, 3);

    // 主循环
    while (1)
    {

        if (touch_read(fd, &tp) > 0)
        {
            // 边缘检测：上次未按下，这次按下 = 新点击
            if (tp.pressed > 0 && last_pressed == 0)
            {
                printf("点击: (%d, %d)\n", tp.x, tp.y);

                // 检查登录区域
                if (tp.x >= 50 && tp.x <= 270 && tp.y >= 80 && tp.y <= 300)
                {
                    printf("进入充值页面\n");
                    user_recharge(fd, account, account_len);
                    // 重新绘制界面
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(40, 100, "recharge1.jpg");
                    show_jpeg(290, 100, "boosting.jpg");
                    show_jpeg(540, 100, "call.jpg");
                    show_jpeg(0, 400, "return2.jpg");
                    Display_characterX(40, 20, welcome_msg, 0xFF000000, 3);
                }
                // 检查打手选择区域
                else if (tp.x >= 290 && tp.x <= 510 && tp.y >= 80 && tp.y <= 300)
                {
                    printf("进入打手选择页面\n");
                    user_select(fd, account, account_len);
                    // 重新绘制界面
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(40, 100, "recharge1.jpg");
                    show_jpeg(290, 100, "boosting.jpg");
                    show_jpeg(540, 100, "call.jpg");
                    show_jpeg(0, 400, "return2.jpg");
                    Display_characterX(40, 20, welcome_msg, 0xFF000000, 3);
                }
                // 检查呼叫客服区域
                else if (tp.x >= 540 && tp.x <= 760 && tp.y >= 80 && tp.y <= 300)
                {
                    printf("进入客服呼叫页面\n");
                    user_call(fd, account, account_len);
                    // 重新绘制界面
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(40, 100, "recharge1.jpg");
                    show_jpeg(290, 100, "boosting.jpg");
                    show_jpeg(540, 100, "call.jpg");
                    show_jpeg(0, 400, "return2.jpg");
                    Display_characterX(40, 20, welcome_msg, 0xFF000000, 3);
                }
                // 检查返回
                else if (tp.x > 20 && tp.x < 780 && tp.y > 380 && tp.y < 480)
                {
                    printf("返回\n");
                    return;
                }
            }

            last_pressed = tp.pressed; // 更新状态
        }

        usleep(10000); // 10ms 降低CPU占用
    }
}

void user_reg_screen(int fd)
{
    TouchPoint tp;
    int account[24] = {0};  // 存储账号
    int password[24] = {0}; // 存储密码
    int account_len = 0;    // 账号位数
    int password_len = 0;   // 密码位数
    int input_mode = 0;     // 0:无输入, 1:输入账号, 2:输入密码

    // 清屏
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);

    // 标题
    Display_characterX(300, 20, "用户注册", 0xFF000000, 3);

    // 账号输入框标签
    Display_characterX(150, 140, "账号:", 0xFF000000, 2);
    // 账号输入框背景
    Clean_Area(250, 130, 300, 50, 0xFFE0E0E0);

    // 密码输入框标签
    Display_characterX(150, 210, "密码:", 0xFF000000, 2);
    // 密码输入框背景
    Clean_Area(250, 200, 300, 50, 0xFFE0E0E0);

    // 确认注册按钮
    Clean_Area(580, 160, 120, 60, 0xFF00AA00);
    Display_characterX(595, 175, "确认", 0xFFFFFFFF, 2);

    // 返回按钮（底部）
    show_jpeg(-20, 50, "return1.jpg");

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
                printf("注册页面点击: (%d, %d)\n", tp.x, tp.y);

                // 账号输入框区域
                if (tp.x >= 250 && tp.x <= 550 && tp.y >= 130 && tp.y <= 200)
                {
                    printf("点击账号输入框1\n");
                    show_number_keyboard();

                    input_mode = 1;

                    // 调用输入函数
                    account_len = input_key_password(tp, fd, 250, 130, account);
                    printf("账号输入完成，共%d位\n", account_len);
                    input_mode = 0;
                }

                // 密码输入框区域
                else if (tp.x >= 250 && tp.x <= 550 && tp.y >= 200 && tp.y <= 250)
                {
                    printf("点击密码输入框\n");
                    show_number_keyboard();
                    input_mode = 2;

                    // 调用输入函数
                    password_len = input_key_password(tp, fd, 250, 200, password);
                    printf("密码输入完成，共%d位\n", password_len);
                    input_mode = 0;
                }

                // 确认按钮区域
                else if (tp.x >= 580 && tp.x <= 700 && tp.y >= 160 && tp.y <= 220)
                {
                    printf("点击确认注册\n");

                    // 检查是否已输入账号和密码
                    if (account_len > 0 && password_len > 0)
                    {
                        // TO: 保存账号密码到文件/数据库
                        save_account_to_file(account, account_len, password, password_len);

                        printf("注册: 账号长度%d, 密码长度%d\n", account_len, password_len);

                        // 注册成功提示
                        Display_characterX(100, 300, "注册成功!返回即可登录", 0xFF00AA00, 2);
                        sleep(1);
                    }
                    else
                    {
                        Display_characterX(200, 300, "请先输入账号和密码", 0xFFFF0000, 2);
                        sleep(1);
                        Clean_Area(200, 300, 400, 40, 0xFFFFFFFF);
                    }
                }

                // 返回按钮区域
                else if (tp.x > 0 && tp.x < 210 && tp.y > 50 && tp.y < 360)
                {
                    printf("返回\n");
                    return;
                }

                else if (input_mode == 0)
                {
                    // 点其他区域且不在输入模式时，清理键盘
                    Clean_Area(0, 320, 800, 160, 0xFFFFFFFF);
                }
            }

            last_pressed = tp.pressed;
        }

        usleep(10000);
    }
}

// 输入键盘数字，返回输入的数字个数
int input_key_password(TouchPoint tp, int fd, int x, int y, int *password)
{
    int input_count = 0;
    char display_str[32];

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

                // 退出条件：点击键盘上方区域 (y < 240)
                if (tp.y < 240)
                {
                    return input_count;
                }

                int num = get_key_number(tp.x, tp.y);
                printf("数字: %d\n", num);
                printf("点击: (%d, %d)\n", tp.x, tp.y);

                // 数字键 0-9
                if (num >= 0 && num <= 9)
                {
                    if (input_count < 24)
                    {
                        password[input_count] = num;
                        input_count++;

                        // 显示 *
                        for (int i = 0; i < input_count; i++)
                            display_str[i] = '*';
                        display_str[input_count] = '\0';

                        Clean_Area(x, y, 280, 40, 0xFFE0E0E0);
                        Display_characterX(x + 10, y + 5, display_str, 0xFF000000, 2);
                    }
                }
            }

            last_pressed = tp.pressed;
        }

        usleep(10000);
    }
}

int input_key_account(TouchPoint tp, int fd, int x, int y, int *password)
{
    int input_count = 0;
    char display_str[32];
    int i; // 循环变量放外面（嵌入式兼容）

    /* 重置状态！关键修复 */
    last_pressed = 1; // 设为1，等待第一次按下变0再变1的完整过程
    /* 或者 last_pressed = 0; 取决于你的触摸逻辑 */

    while (1)
    {
        if (touch_read(fd, &tp) > 0)
        {
            unsigned long now = get_timestamp_ms();

            /* 边缘检测 + 500ms防抖 */
            if (tp.pressed > 0 && last_pressed == 0 &&
                (now - last_click_time) > 500)
            {
                last_click_time = now;

                /* 退出条件：点击键盘下方区域 (y > 240) */
                if (tp.y > 240)
                {
                    printf("exit_keyboard\n");
                    last_pressed = tp.pressed; /* 更新状态再返回 */
                    return input_count;
                }

                int num = get_key_number1(tp.x, tp.y);
                printf("num: %d\n", num);
                printf("point: (%d, %d)\n", tp.x, tp.y);

                /* 数字键 0-9 */
                if (num >= 0 && num <= 9)
                {
                    if (input_count < 24) /* 240太大，改为24 */
                    {
                        password[input_count] = num;
                        input_count++;

                        /* 构造显示字符串 */
                        for (i = 0; i < input_count; i++) /* 用 < 不是 <= */
                        {
                            display_str[i] = '0' + password[i];
                        }
                        display_str[input_count] = '\0'; /* 结束符 */

                        Clean_Area(x, y, 280, 40, 0xFFE0E0E0);
                        Display_characterX(x + 10, y + 5, display_str, 0xFF000000, 2);
                    }
                }
            }

            last_pressed = tp.pressed; /* 每次都要更新 */
        }

        usleep(10000);
    }
}

// int input_key_account(TouchPoint tp, int fd, int x, int y, int *password)
// {
//     int input_count = 0;
//     char display_str[32];
//     tp.pressed = 0;

//     while (1)
//     {
//         if (touch_read(fd, &tp) > 0)
//         {

//             unsigned long now = get_timestamp_ms();

//             // 边缘检测 + 500ms防抖
//             if (tp.pressed > 0 && last_pressed == 0 &&
//                 (now - last_click_time) > 500)
//             {
//                 last_click_time = now;

//                 // 退出条件：点击键盘下方区域 (y < 240)
//                 if (tp.y > 240)
//                 {
//                     printf("exit_keyboad\n");
//                     return input_count;
//                 }

//                 int num = get_key_number1(tp.x, tp.y);
//                 printf("num: %d\n", num);
//                 printf("point: (%d, %d)\n", tp.x, tp.y);

//                 // 数字键 0-9
//                 if (num >= 0 && num <= 9)
//                 {
//                     if (input_count < 240)
//                     {
//                         password[input_count] = num;

//                         // 显示 *
//                         for (int i = 0; i <= input_count; i++)
//                         {
//                             display_str[i] = '0' + password[i];
//                             display_str[input_count + 1] = '\0';
//                         }

//                         input_count++;

//                         Clean_Area(x, y, 280, 40, 0xFFE0E0E0);
//                         Display_characterX(x + 10, y + 5, display_str, 0xFF000000, 2);
//                     }
//                 }
//             }

//             last_pressed = tp.pressed;
//         }

//         usleep(10000);
//     }
// }

void user_login_screen(int fd)
{
    TouchPoint tp;
    static int last_pressed = 1; // 记录上次状态（必须是static或全局）

    // 初始化显示
    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
    show_jpeg(50, 80, "login1.jpg");
    show_jpeg(450, 80, "reg1.jpg");
    show_jpeg(0, 400, "return2.jpg");
    Display_characterX(300, 20, "用户登录", 0xFF000000, 3);

    while (1)
    {

        if (touch_read(fd, &tp) > 0)
        {
            // 边缘检测：上次未按下，这次按下 = 新点击
            if (tp.pressed > 0 && last_pressed == 0)
            {
                printf("点击: (%d, %d)\n", tp.x, tp.y);

                // 检查登录区域
                if (is_admin_area(tp.x, tp.y))
                {
                    printf("进入登录\n");
                    user_login_main(fd);
                    // 重新绘制界面
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(50, 80, "login1.jpg");
                    show_jpeg(450, 80, "reg1.jpg");
                    show_jpeg(0, 400, "return2.jpg");
                    Display_characterX(300, 20, "用户登录", 0xFF000000, 3);
                }
                // 检查注册区域
                else if (is_user_area(tp.x, tp.y))
                {
                    printf("进入注册\n");
                    user_reg_screen(fd);
                    // 重新绘制界面
                    Clean_Area(0, 0, 800, 480, 0xFFFFFFFF);
                    show_jpeg(50, 80, "login1.jpg");
                    show_jpeg(450, 80, "reg1.jpg");
                    show_jpeg(0, 400, "return2.jpg");
                    Display_characterX(300, 20, "用户登录", 0xFF000000, 3);
                }
                // 检查返回
                else if (tp.x > 20 && tp.x < 780 && tp.y > 380 && tp.y < 480)
                {
                    printf("返回\n");
                    return;
                }
            }

            last_pressed = tp.pressed; // 更新状态
        }

        usleep(10000); // 10ms 降低CPU占用
    }
}

int is_user_area(int x, int y)
{
    return (x >= 420 && x <= 650 && y >= 180 && y <= 300);
}

// 显示所有账号
int show_all_accounts(void)
{
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        printf("文件不存在或无法打开\n");
        return -1;
    }

    char line[256];
    printf("\n=== MAN!WHAT CAN I SAY ===\n");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }
    printf("==================\n\n");

    fclose(fp);
    return 0;
}

// 保存账号密码
int save_account_to_file(int *account, int account_len, int *password, int password_len)
{
    FILE *fp = fopen("user_data.txt", "a+"); // 追加模式
    if (fp == NULL)
    {
        printf("打开文件失败!\n");
        return -1;
    }

    // 写入账号
    fprintf(fp, "account:");
    for (int i = 0; i < account_len; i++)
    {
        fprintf(fp, "%d", account[i]);
    }

    // 写入密码
    fprintf(fp, ",password:");
    for (int i = 0; i < password_len; i++)
    {
        fprintf(fp, "%d", password[i]);
    }

    // 注册就送240原石
    fprintf(fp, ",recharge:");
    fprintf(fp, "%d", 24);

    fprintf(fp, "\n"); // 换行，方便下次追加

    fclose(fp);
    printf("账号密码已保存到 user_data.txt\n");
    return 0;
}

// 用户登录验证
// 返回 1 表示验证成功，0 表示失败
int verify_account(int *account, int account_len, int *password, int password_len)
{
    FILE *fp = fopen("user_data.txt", "r");
    if (fp == NULL)
    {
        printf("文件不存在\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // 解析文件中的账号和密码
        int file_account[24] = {0};
        int file_password[24] = {0};
        int file_account_len = 0;
        int file_password_len = 0;

        char *p = line;

        // 跳过 "account:" 前缀
        if (strncmp(p, "account:", 8) != 0)
            continue;
        p += 8;

        // 读取账号数字
        while (*p >= '0' && *p <= '9' && file_account_len < 24)
        {
            file_account[file_account_len++] = *p - '0';
            p++;
        }
        printf("readaccount\n");
        // 跳过 ",password:" 前缀
        if (strncmp(p, ",password:", 10) != 0) // 孩子们，gb2312每个汉字字节不一样,gb2312不要用strncmp！
            continue;
        p += 10;

        // 读取密码数字
        while (*p >= '0' && *p <= '9' && file_password_len < 13)
        {
            file_password[file_password_len++] = *p - '0';
            p++;
        }
        printf("readpassword\n");

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

        // 比较密码
        if (password_len != file_password_len)
            continue;
        int password_match = 1;
        for (int i = 0; i < password_len; i++)
        {
            if (password[i] != file_password[i])
            {
                password_match = 0;
                break;
            }
        }
        printf("comparepassword\n");

        if (account_match && password_match)
        {
            fclose(fp);
            return 1; // 验证成功
        }
    }

    fclose(fp);
    return 0; // 验证失败
}

void user_recharge(int fd, int *account, int account_len)
{
    TouchPoint tp;
    show_jpeg(0, 0, "recharge2.jpg");
    show_jpeg(50, 400, "return3.jpg");
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
                else if (tp.x > 0 && tp.x < 267 && tp.y > 0 && tp.y < 215)
                {
                    printf("充值60原石\n");
                    // 氪金60原石
                    int ret = modify_recharge_by_account(fd, 60, account, account_len);
                    printf("%d\n", ret);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
                else if (tp.x > 267 && tp.x < 541 && tp.y > 0 && tp.y < 215)
                {
                    // 氪金320原石
                    printf("充值320原石\n");
                    modify_recharge_by_account(fd, 320, account, account_len);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
                else if (tp.x > 541 && tp.x < 800 && tp.y > 0 && tp.y < 215)
                {
                    // 氪金750原石
                    printf("充值750原石\n");
                    modify_recharge_by_account(fd, 750, account, account_len);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
                else if (tp.x > 0 && tp.x < 267 && tp.y > 0 && tp.y < 215)
                {
                    // 氪金1480原石
                    printf("充值1480原石\n");
                    modify_recharge_by_account(fd, 1480, account, account_len);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
                else if (tp.x > 267 && tp.x < 541 && tp.y > 215 && tp.y < 400)
                {
                    // 氪金3950原石
                    printf("充值3980原石\n");
                    modify_recharge_by_account(fd, 3950, account, account_len);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
                else if (tp.x > 541 && tp.x < 800 && tp.y > 215 && tp.y < 400)
                {
                    // 氪金8100原石
                    printf("充值8100原石\n");
                    modify_recharge_by_account(fd, 8100, account, account_len);
                    request_call(account, account_len, 0);
                    show_jpeg(325, 200, "recharge3.jpg");
                    sleep(1);
                    return;
                }
            }
        }
    }
}

void user_select(int fd, int *account, int account_len)
{

    int input_mode = 0;
    TouchPoint tp;
    int number[100];
    int number_len;
    char num_char[100];          // 准备数字转字符
    static int last_pressed = 1; // 记录上次状态（必须是static或全局）

    // 初始化显示
    show_jpeg(0, 0, "boosting_main.jpg");
    /* 绘制按钮背景（绿色） */
    Clean_Area(600, 370, 150, 45, 0xFF00AA00);
    /* 显示按钮文字 */
    Display_characterX(615, 380, "查询所有打手", 0xFFFFFFFF, 1);
    while (1)
    {

        if (touch_read(fd, &tp) > 0)
        {
            // 边缘检测：上次未按下，这次按下 = 新点击
            if (tp.pressed > 0 && last_pressed == 0)
            {
                printf("kick: (%d, %d)\n", tp.x, tp.y);

                // 返回按钮（放最前面，优先判断）=
                if (tp.x > 0 && tp.x < 160 && tp.y > 400 && tp.y < 480)
                {
                    printf("返回\n");
                    return;
                }

                // 点击输入区域
                else if (tp.x > 250 && tp.x < 560 && tp.y > 370 && tp.y < 420)
                {
                    printf("input number:\n");
                    show_jpeg(0, 0, "keyboard.jpg");
                    input_mode = 1;
                    number_len = input_key_account(tp, fd, 250, 370, number);
                    printf("编号输入完成，共%d位\n", number_len);
                    input_mode = 0;
                }

                // 确认按钮
                else if (tp.x >= 295 && tp.x <= 505 && tp.y > 420 && tp.y <= 480)
                {
                    printf("点击确认查询\n");
                    if (number_len > 0)
                    {
                        printf("IDwordvity3\n");
                        if (verify_boosting(number, number_len))
                        {
                            printf("IDwordvity4\n");
                            for (int i = 0; i < number_len; i++)
                            {
                                num_char[i] = '0' + number[i];
                            }
                            num_char[number_len] = '\0';
                            // num变成字符串
                            printf("IDwordvity5\n");
                            boosting_screen(fd, num_char, account, account_len);
                            show_jpeg(0, 0, "boosting_main.jpg");
                            /* 绘制按钮背景（绿色） */
                            Clean_Area(600, 370, 150, 45, 0xFF00AA00);
                            /* 显示按钮文字 */
                            Display_characterX(615, 380, "显示所有打手", 0xFFFFFFFF, 1);
                        }
                    }
                }

                else if (tp.x >= 600 && tp.x <= 750 && tp.y > 370 && tp.y <= 430)
                {
                    // 显示所有打手
                    show_all_boosters(fd, account, account_len);
                    show_jpeg(0, 0, "boosting_main.jpg");
                    /* 绘制按钮背景（绿色） */
                    Clean_Area(600, 370, 150, 45, 0xFF00AA00);
                    /* 显示按钮文字 */
                    Display_characterX(615, 380, "显示所有打手", 0xFFFFFFFF, 1);
                }

                // 清理键盘（范围要避开返回按钮）
                else if (input_mode == 0 && tp.y > 200 && tp.y <= 400)
                {
                    show_jpeg(0, 0, "boosting_main.jpg");
                    /* 绘制按钮背景（绿色） */
                    Clean_Area(600, 370, 150, 45, 0xFF00AA00);
                    /* 显示按钮文字 */
                    Display_characterX(615, 380, "显示所有打手", 0xFFFFFFFF, 1);
                }
            }

            last_pressed = tp.pressed;
        }
    }

    usleep(10000); // 10ms 降低CPU占用
}

void user_call(int fd, int *account, int account_len)
{
    TouchPoint tp;
    FILE *fp;
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
                if (tp.x > 0 && tp.x < 120 && tp.y > 320 && tp.y < 480)
                {
                    request_call(account, account_len, 1);
                }
            }
        }
        last_pressed = tp.pressed;
    }
}

void show_all_boosters(int fd, int *account, int account_len)
{
    TouchPoint tp;
    FILE *fp;
    char line[512];
    int current_y;
    int i;
    int len;
    int input_mode = 0;
    int number1[100];
    int number_len;
    char num_char[100]; // 准备数字转字符

    /* 刷新整个屏幕 */
    show_jpeg(0, 0, "boosting_screen.jpg");
    show_jpeg(250, 365, "key.jpg");
    show_jpeg(50, 400, "return3.jpg");

    /* 打开文件 */
    fp = fopen("boosting_data.txt", "r");
    if (fp == NULL)
    {
        /* 文件打开失败，显示错误信息 */
        Display_characterX(30, 30, "无法打开文件", 0xFFFFFFFF, 2);
        return;
    }

    /* 从第一行 (30, 30) 开始 */
    current_y = 30;

    /* 逐行读取并显示 */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /* 去除行尾换行符 */
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
            continue;
        }

        /* 显示当前行，x=30，字体大小2，白色 */
        Display_characterX(30, current_y, (unsigned char *)line, 0xFFFFFFFF, 1);

        /* y 坐标增加 70 */
        current_y = current_y + 50;

        /* 防止超出屏幕底部（简单保护） */
        if (current_y > 450)
        {
            break;
        }
    }
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
                else if (tp.x > 250 && tp.x < 560 && tp.y > 370 && tp.y < 420)
                {
                    printf("input number:\n");
                    show_jpeg(0, 0, "keyboard.jpg");
                    input_mode = 1;
                    number_len = input_key_account(tp, fd, 250, 370, number1);
                    printf("编号输入完成，共%d位\n", number_len);
                    input_mode = 0;
                }

                // 确认按钮
                else if (tp.x >= 295 && tp.x <= 505 && tp.y > 420 && tp.y <= 480)
                {
                    printf("confirm\n");
                    if (number_len > 0)
                    {
                        printf("IDwordvity3\n");
                        if (verify_boosting(number1, number_len))
                        {
                            printf("IDwordvity4\n");
                            for (int i = 0; i < number_len; i++)
                            {
                                num_char[i] = '0' + number1[i];
                            }
                            num_char[number_len] = '\0';
                            // num变成字符串
                            printf("IDwordvity5\n");
                            boosting_screen(fd, num_char, account, account_len);
                            printf("IDwordvity6\n");
                            show_jpeg(0, 0, "boosting_screen.jpg");
                            show_jpeg(50, 400, "return3.jpg");
                        }
                        else
                        {
                            number_len = 0;
                            show_jpeg(250, 365, "key.jpg");
                            Display_characterX(300, 320, "manbaOUT!", 0xFFFF0000, 4);
                        }
                    }
                }
            }
        }
        last_pressed = tp.pressed;
    }
    fclose(fp);
}
