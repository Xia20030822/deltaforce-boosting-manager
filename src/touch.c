#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdlib.h>
#include "touch.h"

int touch_init(const char *dev_path)
{
    int fd = open(dev_path, O_RDONLY);

    if (fd < 0)
    {
        perror("touch open failed");
        return -1;
    }

    return fd;
}

int touch_read(int fd, TouchPoint *tp)
{
    static int tx = 0, ty = 0; // 静态变量
    int pressed = 0;

    struct input_event ev;

    ssize_t n = read(fd, &ev, sizeof(ev));

    if (n != sizeof(ev))
        return 0;

    if (ev.type == EV_ABS)
    {
        if (ev.code == ABS_X)
            tx = ev.value;

        if (ev.code == ABS_Y)
            ty = ev.value;
    }

    if (ev.type == EV_KEY &&
        ev.code == BTN_TOUCH)
    {
        pressed = ev.value;
    }

    tp->x = tx * LCD_W / TOUCH_W;
    tp->y = ty * LCD_H / TOUCH_H_VAL;
    tp->pressed = pressed;
    // printf("press:%d\n", pressed);

    return 1;
}

SwipeDirection get_swipe_direction(int x, int y, int pressed)
{
    static int start_x, start_y;
    static int tracking = 0;

    int threshold = 30;

    if (pressed && !tracking)
    {
        start_x = x;
        start_y = y;
        tracking = 1;
    }
    else if (!pressed && tracking)
    {
        int dx = x - start_x;
        int dy = y - start_y;

        tracking = 0;

        if (abs(dx) < threshold &&
            abs(dy) < threshold)
            return SWIPE_NONE;

        if (abs(dx) > abs(dy))
        {
            if (dx > 0)
                return SWIPE_RIGHT;
            else
                return SWIPE_LEFT;
        }
        else
        {
            if (dy > 0)
                return SWIPE_DOWN;
            else
                return SWIPE_UP;
        }
    }

    return SWIPE_NONE;
}

void touch_close(int fd)
{
    if (fd > 0)
        close(fd);
}