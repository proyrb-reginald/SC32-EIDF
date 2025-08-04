#ifndef IRQ_H
#define IRQ_H

/**
 * @brief 提供中断处理函数。
 * @details
 * @file irq.h
 * @author proyrb
 * @date 2025/7/30
 * @note
 */

/********** 导入需要的头文件 **********/

#include "sc32_conf.h"

/********** 导出的函数 **********/

/**
 * @brief dma通道0中断处理函数。
 * @param
 * @retval
 * @warning 需要开启对应中断使能；不允许非中断下调用。
 * @note
 */
void DMA0_IRQHandler(void);

/**
 * @brief dma通道1中断处理函数。
 * @param
 * @retval
 * @warning 需要开启对应中断使能；不允许非中断下调用。
 * @note
 */
void DMA1_IRQHandler(void);

/**
 * @brief dma通道2中断处理函数。
 * @param
 * @retval
 * @warning 需要开启对应中断使能；不允许非中断下调用。
 * @note
 */
void DMA2_IRQHandler(void);

#endif  // IRQ_H
