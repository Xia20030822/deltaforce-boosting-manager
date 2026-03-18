#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "lcd.h"

unsigned int *fbp;
int screen_width;
int screen_height;
long screensize;

void init_fb()
{
    int fb = open("/dev/fb0", O_RDWR);
    if (fb < 0)
    {
        perror("open fb0");
        exit(1);
    }

    struct fb_var_screeninfo vinfo;

    ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);

    screen_width = vinfo.xres;
    screen_height = vinfo.yres;

    screensize = screen_width * screen_height * 4;

    fbp = mmap(0, screensize,
               PROT_READ | PROT_WRITE,
               MAP_SHARED, fb, 0);

    if (fbp == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
}