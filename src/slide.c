#include <string.h>
#include <unistd.h>
#include "lcd.h"
#include "slide.h"

// 通用滑动函数
void slide_image(unsigned int *img_buf, int img_width, int img_height,
                 SwipeDirection dir, int step)
{
    int max_offset_x, max_offset_y, offset_x, offset_y;

    // 设置初始和最大偏移
    switch (dir)
    {
    case SWIPE_UP:
        offset_y = screen_height; // 图片完全在屏幕下方
        max_offset_y = -img_height;
        while (offset_y >= max_offset_y)
        {
            for (int y = 0; y < img_height; y++)
            {
                int screen_y = y + offset_y - img_height;
                if (screen_y < 0 || screen_y >= screen_height)
                    continue;

                int copy_width = (img_width < screen_width ? img_width : screen_width);
                memcpy(fbp + screen_y * screen_width,
                       img_buf + y * img_width,
                       sizeof(unsigned int) * copy_width);
                for (int x = copy_width; x < screen_width; x++)
                    fbp[screen_y * screen_width + x] = 0xFF000000;
            }
            offset_y -= step;
            usleep(500);
        }
        break;

    case SWIPE_DOWN:
        offset_y = -img_height; // 图片完全在屏幕上方
        max_offset_y = screen_height;
        while (offset_y <= max_offset_y)
        {
            for (int y = 0; y < img_height; y++)
            {
                int screen_y = y + offset_y;
                if (screen_y < 0 || screen_y >= screen_height)
                    continue;

                int copy_width = (img_width < screen_width ? img_width : screen_width);
                memcpy(fbp + screen_y * screen_width,
                       img_buf + y * img_width,
                       sizeof(unsigned int) * copy_width);
                for (int x = copy_width; x < screen_width; x++)
                    fbp[screen_y * screen_width + x] = 0xFF000000;
            }
            offset_y += step;
            usleep(500);
        }
        break;

    case SWIPE_LEFT:
        offset_x = screen_width + img_width; // 图片完全在屏幕右方
        max_offset_x = 0;
        while (offset_x >= max_offset_x)
        {
            for (int y = 0; y < img_height && y < screen_height; y++)
            {
                for (int x = 0; x < img_width && x < screen_width; x++)
                {
                    int screen_x = x + offset_x - img_width;
                    if (screen_x < 0 || screen_x >= screen_width)
                        continue;
                    fbp[y * screen_width + screen_x] = img_buf[y * img_width + x];
                }
            }
            offset_x -= step;
            usleep(500);
        }
        break;

    case SWIPE_RIGHT:
        offset_x = -img_width; // 图片完全在屏幕左方
        max_offset_x = screen_width;
        while (offset_x <= max_offset_x)
        {
            for (int y = 0; y < img_height && y < screen_height; y++)
            {
                for (int x = 0; x < img_width && x < screen_width; x++)
                {
                    int screen_x = x + offset_x;
                    if (screen_x < 0 || screen_x >= screen_width)
                        continue;
                    fbp[y * screen_width + screen_x] = img_buf[y * img_width + x];
                }
            }
            offset_x += step;
            usleep(500);
        }
        break;
    }
}