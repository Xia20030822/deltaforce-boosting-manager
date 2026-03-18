#include <linux/fb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include </home/a/arm-gif/include/gif_lib.h>
#include "user.h"

void show_gif(const char *filename,
              unsigned char *fbp,
              struct fb_var_screeninfo *vinfo)
{
    int bytes = vinfo->bits_per_pixel / 8;
    int error;

    GifFileType *gif = DGifOpenFileName(filename, &error); // 打开gif文件
    if (!gif)
    {
        printf("open gif failed\n");
        return;
    }

    DGifSlurp(gif); // 读取所有帧

    while (1)
    {
        for (int frame = 0; frame < gif->ImageCount; frame++)
        {
            SavedImage *img = &gif->SavedImages[frame]; // 数组，每个元素代表一帧
            GifImageDesc *desc = &img->ImageDesc;       // 图片的详细信息，宽高，颜色

            GifByteType *raster = img->RasterBits;                                   // 获取调色板
            ColorMapObject *cmap = desc->ColorMap ? desc->ColorMap : gif->SColorMap; // 局部调色板和全局调色板

            for (int y = 0; y < desc->Height; y++)
            {
                for (int x = 0; x < desc->Width; x++)
                {
                    int gif_index = y * desc->Width + x;
                    GifColorType color =
                        cmap->Colors[raster[gif_index]];

                    int screen_x = x;
                    int screen_y = y;

                    if (screen_x >= vinfo->xres ||
                        screen_y >= vinfo->yres)
                        continue;

                    int location =
                        (screen_x + screen_y * vinfo->xres_virtual) * bytes;

                    fbp[location + 0] = color.Blue;
                    fbp[location + 1] = color.Green;
                    fbp[location + 2] = color.Red;
                    fbp[location + 3] = 0;
                }
            }

            usleep(20000); // 每帧延时 80ms
        }
    }

    DGifCloseFile(gif, &error);
}