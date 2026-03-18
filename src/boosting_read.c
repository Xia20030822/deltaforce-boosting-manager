/**
 * boosting_lookup.c
 * 从 boosting_data.txt 中根据 number 查找对应行的数据 (嵌入式兼容版本)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boosting_read.h"

#define MAX_LINE_LENGTH 512

/**
 * 去除字符串首尾空格
 */
static void trim(char *str)
{
    char *start = str;
    char *end;

    while (*start == ' ' || *start == '\t')
        start++;

    if (*start == '\0') {
        str[0] = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }

    memmove(str, start, end - start + 1);
    str[end - start + 1] = '\0';
}

/**
 * 从 boosting_data.txt 中根据 number 查找数据
 */
int get_boosting_by_number(char *number,
                           char *price,
                           char *level,
                           char *photo_name,
                           char *gif_name,
                           char *name,
                           int *time)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int found = 0;
    int i;

    fp = fopen("boosting_data.txt", "r");
    if (fp == NULL) {
        return -1;  /* 文件打开失败 */
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char buffer[MAX_LINE_LENGTH];
        char *token;
        char *colon;
        char *key;
        char *value;

        char current_number[32] = {0};
        char current_price[64] = {0};
        char current_level[16] = {0};
        char current_photo[128] = {0};
        char current_gif[128] = {0};
        char current_name[64] = {0};
        int current_time = 0;

        /* 去除行尾换行符 */
        for (i = 0; line[i] != '\0'; i++) {
            if (line[i] == '\n' || line[i] == '\r') {
                line[i] = '\0';
                break;
            }
        }

        /* 跳过空行 */
        if (strlen(line) == 0)
            continue;

        /* 检查是否以 "number:" 开头 */
        if (strncmp(line, "number:", 7) != 0)
            continue;

        /* 解析当前行 */
        strncpy(buffer, line, MAX_LINE_LENGTH - 1);
        buffer[MAX_LINE_LENGTH - 1] = '\0';

        /* 按逗号分割 */
        token = strtok(buffer, ",");
        while (token != NULL) {
            colon = strchr(token, ':');
            if (colon != NULL) {
                *colon = '\0';
                key = token;
                value = colon + 1;

                trim(key);
                trim(value);

                if (strcmp(key, "number") == 0) {
                    strncpy(current_number, value, sizeof(current_number) - 1);
                    current_number[sizeof(current_number) - 1] = '\0';
                } else if (strcmp(key, "price") == 0) {
                    strncpy(current_price, value, sizeof(current_price) - 1);
                    current_price[sizeof(current_price) - 1] = '\0';
                } else if (strcmp(key, "level") == 0) {
                    strncpy(current_level, value, sizeof(current_level) - 1);
                    current_level[sizeof(current_level) - 1] = '\0';
                } else if (strcmp(key, "photo") == 0) {
                    strncpy(current_photo, value, sizeof(current_photo) - 1);
                    current_photo[sizeof(current_photo) - 1] = '\0';
                } else if (strcmp(key, "gif") == 0) {
                    strncpy(current_gif, value, sizeof(current_gif) - 1);
                    current_gif[sizeof(current_gif) - 1] = '\0';
                } else if (strcmp(key, "name") == 0) {
                    strncpy(current_name, value, sizeof(current_name) - 1);
                    current_name[sizeof(current_name) - 1] = '\0';
                } else if (strcmp(key, "time") == 0) {
                    current_time = atoi(value);
                }
            }
            token = strtok(NULL, ",");
        }

        /* 比较 number 是否匹配 */
        if (strcmp(current_number, number) == 0) {
            /* 找到匹配，复制数据到传出参数 */
            if (price) {
                strncpy(price, current_price, 63);
                price[63] = '\0';
            }
            if (level) {
                strncpy(level, current_level, 15);
                level[15] = '\0';
            }
            if (photo_name) {
                strncpy(photo_name, current_photo, 127);
                photo_name[127] = '\0';
            }
            if (gif_name) {
                strncpy(gif_name, current_gif, 127);
                gif_name[127] = '\0';
            }
            if (name) {
                strncpy(name, current_name, 63);
                name[63] = '\0';
            }
            if (time) {
                *time = current_time;
            }
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (!found) {
        /* 未找到，清空传出参数 */
        if (price) price[0] = '\0';
        if (level) level[0] = '\0';
        if (photo_name) photo_name[0] = '\0';
        if (gif_name) gif_name[0] = '\0';
        if (name) name[0] = '\0';
        if (time) *time = 0;
        return -2;  /* 未找到 */
    }

    

    return 0;  /* 成功 */
}
