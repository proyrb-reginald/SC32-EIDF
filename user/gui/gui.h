#ifndef GUI_H
#define GUI_H

/**
 * @brief 这是GUI模块
 * @details
 * @file gui.h
 * @author proyrb
 * @date 2025/7/30
 * @note
 */

/********** 导入需要的头文件 **********/

#include "FreeRTOS.h"

/********** 配置GUI选项 **********/

#define GUI_MIN_SLP_MS 3

/********** 导出的函数 **********/

/**
 * @brief 模块初始化函数。
 * @param
 * @retval pdPASS：初始化成功。
 * @warning 初始化完成以后才能运行lvgl处理任务。
 * @note
 */
BaseType_t gui_init(void);

/**
 * @brief lvgl处理专用任务。
 * @param task_arg 任务参数：目前未使用。
 * @retval
 * @warning
 * @note
 */
void gui_task(void * task_arg);

#endif  // GUI_H
