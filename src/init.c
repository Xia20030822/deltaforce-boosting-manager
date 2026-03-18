#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "jpeg.h"
#include "lcd.h"
#include "slide.h"
#include "touch.h"
#include "showvideo.h"

int init_all()
{
    init_fb();
    int fd = touch_init("/dev/input/event0");
    if (fd < 0)
    {
        return fd;
    }
    return fd;
}
