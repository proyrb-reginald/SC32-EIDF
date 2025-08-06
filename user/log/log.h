#ifndef LOG_H
#define LOG_H

/**
 * @brief 这是日志模块。
 * @details
 * @file log.h
 * @author proyrb
 * @date 2025/8/6
 * @note
 */

/********** 导入需要的头文件 **********/

#include <sc32_conf.h>
#include <stdio.h>
#include <string.h>

/********** 选择 log 配置 **********/

// 日志等级
#define TRAC_LOG 0
#define INFO_LOG 1
#define NEWS_LOG 2
#define WARN_LOG 3
#define ERRO_LOG 4
#define NONE_LOG 5

// 日志输出阈值（输出大于等于该级别的日志）
#define LOG_LEV NEWS_LOG

// 格式化打印接口函数
#define PRINTF printf

// 获取系统时刻接口函数
#define GET_TICK RCC_SystickGetCounter

// 获取文件名
#define __FILENAME__                                                                     \
    (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/') + 1 :                              \
     strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 :                             \
                               __FILE__)

// 普通日志打印
#define PRTF(lev, fmt, ...)                                                              \
    if (lev >= LOG_LEV) {                                                                \
        PRINTF(fmt, ##__VA_ARGS__);                                                      \
    }

// 附带系统信息的日志打印
#define OS_PRTF(lev, fmt, ...)                                                           \
    PRTF(lev, "[%u:%s:%d] " fmt "\n", GET_TICK(), __FILENAME__, __LINE__, ##__VA_ARGS__);

// 断言
#define configASSERT(x)                                                                  \
    if (x) {                                                                             \
        while (1) {}                                                                     \
    }

#endif  // LOG_H
