#ifndef TOUCH_H
#define TOUCH_H

#ifndef LCD_W
#define LCD_W 800
#endif

#ifndef LCD_H
#define LCD_H 480
#endif

#ifndef TOUCH_W
#define TOUCH_W 1024
#endif

#ifndef TOUCH_H_VAL
#define TOUCH_H_VAL 600
#endif

int last_pressed;
unsigned long last_click_time;

typedef struct
{
    int x;
    int y;
    int pressed;
} TouchPoint;

typedef enum
{
    SWIPE_NONE,
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT
} SwipeDirection;

int touch_init(const char *dev_path);
int touch_read(int fd, TouchPoint *tp);
void touch_close(int fd);

SwipeDirection get_swipe_direction(int x, int y, int pressed);

#endif
