#ifndef SLIDE_H
#define SLIDE_H

#include "touch.h"

void slide_image(unsigned int *img_buf,
                 int img_width,
                 int img_height,
                 SwipeDirection dir,
                 int step);

#endif