#ifndef JPEG_H
#define JPEG_H

unsigned int *show_jpeg_to_buf(const char *filename, int *out_width, int *out_height);
void show_jpeg(int x0, int y0, const char *filename);

#endif