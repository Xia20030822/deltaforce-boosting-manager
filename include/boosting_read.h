/**
 * boosting_lookup.h
 * 从 boosting_data.txt 中根据 number 查找对应行的数据 (嵌入式兼容版本)
 */

#ifndef BOOSTING_LOOKUP_H
#define BOOSTING_LOOKUP_H
/* 外部字体函数声明 */


/**
 * 从 boosting_data.txt 中根据 number 查找数据
 *
 * 参数:
 *   number     - 要查找的编号 (如 "24")
 *   price      - 传出参数，返回价格
 *   level      - 传出参数，返回等级
 *   photo_name - 传出参数，返回照片文件名
 *   gif_name   - 传出参数，返回 GIF 文件名
 *   name       - 传出参数，返回名称
 *   time       - 传出参数，返回时间状态 (0=空闲, 1=忙碌)
 *
 * 返回:
 *   0  - 成功找到
 *   -1 - 文件打开失败
 *   -2 - 未找到该 number
 *
 * 使用示例:
 *   char price[64], level[16], photo[128], gif[128], name[64];
 *   int time;
 *   int ret = get_boosting_by_number("24", price, level, photo, gif, name, &time);
 *   if (ret == 0) {
 *       printf("价格: %s, 状态: %s\n", price, time == 0 ? "空闲" : "忙碌");
 *   }
 */
int get_boosting_by_number(char *number,
                           char *price,
                           char *level,
                           char *photo_name,
                           char *gif_name,
                           char *name,
                           int *time);


#endif /* BOOSTING_LOOKUP_H */
