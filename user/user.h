#ifndef USER_H
#define USER_H

/**
 * @brief 用户启动代码配置
 * @details
 * @file user.h
 * @author proyrb
 * @date 2025/8/4
 * @note
 */

/********** 导入需要的头文件 **********/

/********** 配置项目驱动 **********/

// 是否添加基于W25Q64的相关代码
#define ADD_W25Q64 1

// 是否添加基于ST7789V的相关代码
#define ADD_ST7789V 1

/********** 配置项目组件 **********/

// 是否添加基于片内文件系统的相关代码
#define ADD_EBD_FS 1

// 是否在启动时重置启动信息
#define RST_BOOT_INFO 1

// 是否添加基于片外文件系统的相关代码
#if ADD_W25Q64
#    define ADD_ETN_FS 1
#else
#    define ADD_ETN_FS 0
#endif  // ADD_W25Q64

// 是否添加基于GUI的相关代码
#if ADD_ST7789V
#    define ADD_GUI 1
// 最小睡眠休眠时间
#    define GUI_MIN_SLP_MS 3
#else
#    define ADD_GUI 0
#endif  // ADD_ST7789V

#if ADD_GUI
// 是否使用任务监控视图作为图形界面
#    define GUI_TASK_MONITOR 1
#else
#    define GUI_TASK_MONITOR 0
#endif  // ADD_GUI

#endif  // USER_H
