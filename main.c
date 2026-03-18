#include <stdio.h>
#include "lcd.h"
#include "jpeg.h"
#include "touch.h"
#include "slide.h"
#include "showvideo.h"
#include "font.h"

int admin_password[100] = {1, 2, 3, 4, 5, 6}; // 管理员密码账号唯一且无法更改
// 板子有问题y<170都会直接变成0！！！
int main()
{
    last_pressed = 0;
    last_click_time = 0;

    printf("first tree1!\n");
    Init_Font();
    int fd = init_all();

    play_mp4("1.mp4", fd);
    while (1)
    {
        printf("18\n");
        function(fd);
    }

    UnInit_Font();
    touch_close(fd);

    return 0;
}