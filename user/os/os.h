#ifndef OS_H
#define OS_H

/**
 * @brief 这是一个操作系统模块，封装了rtos部分接口，提供任务内存监视功能。
 * @details
 * 使用方法：实现vApplicationDaemonTaskStartupHook函数，添加用户级的系统任务初始化代码；
 * 在rtos启动调度前调用os_init；
 * 支持任务动态创建并动态调用os_add_task。
 * @file os.h
 * @author proyrb
 * @date 2025/7/27
 * @note
 */

/********** 导入需要的头文件 **********/

#include "FreeRTOS.h"
#include "semphr.h"

/********** 选择 os 配置 **********/

// 系统任务列表的容量
#define SYS_TASK_CNT 8

// 系统任务内存监视器调用间隔，默认ms为单位（非精确调用）
#define MNT_CALL_PERIOD 3000

// 系统模块初始化完成
#define OS_MD_INIT_DONE()                                                                \
    OS_PRTF(NEWS_LOG, "init md done! free: %u W / %u B",                                 \
            uxTaskGetStackHighWaterMark2(NULL), xPortGetFreeHeapSize());                 \
    xSemaphoreGive(os_md_init_done);                                                     \
    vTaskDelete(NULL)

/********** 导出的全局变量 **********/

// 系统模块初始化同步信号量
extern SemaphoreHandle_t os_md_init_done;

// DMA互斥锁
extern SemaphoreHandle_t dma_mutex;

/********** 导出的函数 **********/

/**
 * @brief 需要实现的freertos守护任务的钩子函数。
 * @param
 * @retval
 * @warning 该函数会在调度器启动的早期调用，只调用一次。
 * @note
 */
extern void vApplicationDaemonTaskStartupHook(void);

/**
 * @brief 初始化系统资源。
 * @param
 * @retval pdPASS：初始化成功。
 * @retval pdFAIL：初始化失败，内存分配失败。
 * @warning 在调度器启动前调用一次。
 * @note
 */
BaseType_t os_init(void);

/**
 * @brief 向系统任务内存监视器添加任务。
 * @param task_hdl 需要添加的任务句柄指针。
 * @retval pdPASS：任务添加成功。
 * @retval pdFAIL：任务添加失败，容器已满。
 * @warning 如果监视任务的容器已满则添加失败。
 * @note
 */
BaseType_t os_add_task(TaskHandle_t * const task_hdl);

/**
 * @brief 需要实现的模块初始化函数。
 * @param
 * @retval task_arg：任务参数，目前未使用。
 * @warning 在调度器启动的早期创建，请在实现中的最后执行删除自己的操作。
 * @note
 */
extern void os_md_init(void * task_arg);

/**
 * @brief 主任务创建函数。
 * @param
 * @retval task_arg：任务参数，目前未使用。
 * @warning 创建后会等待模块初始化完成后执行，请在实现中的最后执行删除自己的操作。
 * @note
 */
extern void os_task_init(void * task_arg);

#endif  // OS_H
