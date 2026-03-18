/**
 * recharge_manager.c
 * 根据账号修改余额
 */

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <linux/input.h>
#include <unistd.h>
#include <string.h>
#include "jpeg.h"
#include "lcd.h"
#include "slide.h"
#include "touch.h"
#include "font.h"
#include "user.h"
#include "adm.h"
#include "boosting.h"
#include "boosting_read.h"
/**
 * 根据账号修改充值余额
 *
 * 参数:
 *   tp      - 触碰状态（1=按下，0=松开），用于检测确认操作
 *   recharge- 要增加或减少的金额（正数=充值，负数=扣费）
 *   account - 要查找的账号（如 24）
 *
 * 返回:
 *   0  - 修改成功
 *   -1 - 文件打开失败
 *   -2 - 未找到该账号
 *   -3 - 余额不足（扣费时）
 *   -4 - 临时文件操作失败
 *
 * 使用示例:
 *   // 给账号 24 充值 100
 *   modify_recharge_by_account(1, 100, 24);
 *
 *   // 给账号 24 扣费 50
 *   modify_recharge_by_account(1, -50, 24);
 */
/**
 * recharge_manager.c
 * 根据账号修改余额（账号为数组形式）
 //
 * 将 int 数组转换为整数
 *
 * 参数:
 *   arr - int 数组（如 {2, 4}）
 *   len - 数组长度
 *
 * 返回:
 *   转换后的整数（如 24）
 */
int array_to_int(int *arr, int len)
{
    int result = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        result = result * 10 + arr[i];
    }

    return result;
}

/**
 * 根据账号修改充值余额
 *
 * 参数:
 *   tp          - 触碰状态（1=按下，0=松开），用于检测确认操作
 *   recharge    - 要增加或减少的金额（正数=充值，负数=扣费）
 *   account     - 要查找的账号数组（如 {2, 4} 表示账号 24）
 *   account_len - 账号数组长度
 *
 * 返回:
 *   0  - 修改成功
 *   -1 - 文件打开失败
 *   -2 - 未找到该账号
 *   -3 - 余额不足（扣费时）
 *   -4 - 临时文件操作失败
 *
 * 使用示例:
 *   int acc[] = {2, 4};  // 账号 24
 *   modify_recharge_by_account(1, 100, acc, 2);  // 充值 100
 *   modify_recharge_by_account(1, -50, acc, 2);  // 扣费 50
 */
int modify_recharge_by_account(int tp, int recharge, int *account, int account_len)
{
    FILE *fp;
    FILE *temp_fp;
    char line[512];
    int target_account;
    int found = 0;
    int i;
    int len;
    int result = 0;

    /* 将账号数组转换为整数 */
    target_account = array_to_int(account, account_len);
    printf("recharge_test1\n");

    /* 打开原文件 */
    fp = fopen("user_data.txt", "r");
    if (fp == NULL)
    {
        return -1;
    }

    /* 打开临时文件 */
    temp_fp = fopen("user_data_temp.txt", "w");
    if (temp_fp == NULL)
    {
        fclose(fp);
        return -4;
    }
    printf("recharge_test1\n");

    /* 逐行读取处理 */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int file_account = 0;
        int file_recharge = 0;
        int file_password = 0;
        int new_recharge = 0;
        char *p;
        char *password_start;
        char *recharge_start;

        /* 去除换行符 */
        len = strlen(line);
        for (i = 0; i < len; i++)
        {
            if (line[i] == '\n' || line[i] == '\r')
            {
                line[i] = '\0';
                break;
            }
        }

        /* 跳过空行 */
        if (strlen(line) == 0)
        {
            fprintf(temp_fp, "\n");
            continue;
        }

        /* 解析账号 */
        p = line;
        if (strncmp(p, "account:", 8) == 0)
        {
            p = p + 8;
            file_account = 0;
            while (*p >= '0' && *p <= '9')
            {
                file_account = file_account * 10 + (*p - '0');
                p++;
            }
        }

        /* 检查是否目标账号 */
        if (file_account == target_account)
        {
            found = 1;

            /* 找到 password: 位置，记录 password 值 */
            password_start = strstr(line, "password:");
            if (password_start != NULL)
            {
                password_start = password_start + 9;
                file_password = 0;
                while (*password_start >= '0' && *password_start <= '9')
                {
                    file_password = file_password * 10 + (*password_start - '0');
                    password_start++;
                }
            }

            /* 找到 recharge: 位置 */
            recharge_start = strstr(line, "recharge:");
            if (recharge_start != NULL)
            {
                recharge_start = recharge_start + 9;
                file_recharge = 0;
                while (*recharge_start >= '0' && *recharge_start <= '9')
                {
                    file_recharge = file_recharge * 10 + (*recharge_start - '0');
                    recharge_start++;
                }
            }

            /* 计算新余额 */
            new_recharge = file_recharge + recharge;

            /* 检查余额不足 */
            if (new_recharge < 0)
            {
                result = -3;
                new_recharge = file_recharge;
            }

            /* 构造新行 */
            fprintf(temp_fp, "account:%d,password:%d,recharge:%d\n",
                    target_account, file_password, new_recharge);
        }
        else
        {
            /* 非目标账号，原样写入 */
            fprintf(temp_fp, "%s\n", line);
        }
        printf("recharge_test2\n");
    }
    printf("recharge_test3\n");

    fclose(fp);
    fclose(temp_fp);

    /* 检查是否找到账号 */
    if (!found)
    {
        remove("user_data_temp.txt");
        return -2;
    }

    /* 检查是否有错误 */
    if (result != 0)
    {
        remove("user_data_temp.txt");
        return result;
    }

    /* 替换原文件 */
    remove("user_data.txt");
    rename("user_data_temp.txt", "user_data.txt");
    printf("recharge_test4\n");

    return 0;
}
