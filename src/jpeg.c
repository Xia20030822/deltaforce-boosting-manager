#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "jpeg.h"
#include "lcd.h"

unsigned int *show_jpeg_to_buf(const char *filename,
                               int *out_width,
                               int *out_height)
{
    char filepath[1000];
    sprintf(filepath, "./video/%s", filename);
    FILE *fp = fopen(filepath, "rb");
    if (!fp)
        return NULL;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);

    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;

    unsigned char *tmp_buf = malloc(width * height * 3);

    while (cinfo.output_scanline < height)
    {
        unsigned char *rowptr =
            tmp_buf + cinfo.output_scanline * width * 3;

        jpeg_read_scanlines(&cinfo, &rowptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(fp);

    unsigned int *buf = malloc(width * height * 4);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            unsigned char r = tmp_buf[idx];
            unsigned char g = tmp_buf[idx + 1];
            unsigned char b = tmp_buf[idx + 2];

            buf[y * width + x] =
                (0xFF << 24) | (r << 16) | (g << 8) | b;
        }
    }

    free(tmp_buf);

    *out_width = width;
    *out_height = height;

    return buf;
}

void show_jpeg(int x0, int y0, const char *filename)
{
    int img_w, img_h;

    unsigned int *buf =
        show_jpeg_to_buf(filename, &img_w, &img_h);

    if (!buf)
    {
        printf("jpeg decode failed\n");
        return;
    }

    for (int y = 0; y < img_h; y++)
    {
        for (int x = 0; x < img_w; x++)
        {
            int lcd_x = x0 + x;
            int lcd_y = y0 + y;

            // 防止越界
            if (lcd_x >= 0 && lcd_x < screen_width &&
                lcd_y >= 0 && lcd_y < screen_height)
            {
                fbp[lcd_y * screen_width + lcd_x] =
                    buf[y * img_w + x];
            }
        }
    }

    free(buf);
}