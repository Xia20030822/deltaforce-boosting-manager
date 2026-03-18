#ifndef BOOSTING_H
#define BOOSTING_H
#define MAX_LINE_LENGTH 512
#define MAX_RECORDS 1000
#define MAX_FIELD_LENGTH 128

// 数据结构定义
typedef struct
{
    char number[32];
    char price[64];
    char level[16];
    char photo[MAX_FIELD_LENGTH];
    char gif[MAX_FIELD_LENGTH];
    char name[MAX_FIELD_LENGTH];
} BoostingRecord;

int verify_boosting(int *number, int number_len);
void boosting_screen(int fd, char *num, int *account, int account_len);

#endif
