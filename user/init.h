#ifndef INIT_H
#define INIT_H

/**
 * @brief 提供初始化操作
 * @details
 * @file init.h
 * @author proyrb
 * @date 2025/7/30
 * @note
 */

/********** 导入需要的头文件 **********/

#include "sc32_conf.h"

/********** 导出的函数 **********/

/**
 * @brief 初始化时钟配置，尝试切换到64Mhz，并初始化uart5，使能printf。
 * @param
 * @retval
 * @warning
 * @note
 */
void rcc_init(void);

/**
 * @brief 初始化gpio引脚。
 * @param
 * @retval
 * @warning
 * @note
 */
void gpio_init(void);

/**
 * @brief 初始化uart5引脚。
 * @param
 * @retval
 * @warning 在rcc_init中以及初始化过一次uart5。
 * @note
 */
void uart_5_init(void);

/**
 * @brief 初始化spi2。
 * @param
 * @retval
 * @warning
 * @note
 */
void spi_2_init(void);

/**
 * @brief 初始化qspi0。
 * @param
 * @retval
 * @warning
 * @note
 */
void qspi_0_init(void);

/**
 * @brief 初始化dma通道0。
 * @param
 * @retval
 * @warning
 * @note
 */
void dma_0_init(void);

/**
 * @brief 初始化dma通道1。
 * @param
 * @retval
 * @warning
 * @note
 */
void dma_1_init(void);

/**
 * @brief 初始化dma通道2。
 * @param
 * @retval
 * @warning
 * @note
 */
void dma_2_init(void);

/**
 * @brief 执行上述所有初始化。
 * @param
 * @retval
 * @warning
 * @note
 */
void bsp_init(void);

#endif  // INIT_H
