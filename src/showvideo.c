#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
void play_mp4(const char *filename, int fd)
{
    char filepath[256];
    char cmd[512];
    struct input_event ev;

    sprintf(filepath, "./video/%s", filename);

    // 后台播放
    sprintf(cmd, "mplayer -quiet -vo fbdev2 -zoom -x 800 -y 480 %s &", filepath);
    system(cmd);

    while (1)
    {
        if (read(fd, &ev, sizeof(ev)) <= 0)
            continue;
        if (ev.type == EV_KEY && ev.value == 1)
        {
            printf("关闭视频\n");

            system("killall mplayer");
            break;
        }
    }
}
