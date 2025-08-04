#ifndef EBD_FS_H
#define EBD_FS_H

/**
 * @brief 这是片内文件系统模块。
 * @details 先初始化模块。
 * @file ebd_fs.h
 * @author proyrb
 * @date 2025/7/28
 * @note
 */

/********** 导入需要的头文件 **********/

#include "lfs.h"

/********** 片内空间配置 **********/

#ifdef EBD_FS_C
#    define EBD_BASE_ADR FLASH_BASE
#    define EBD_SIZE 0x40000
#    define EBD_BLK_SIZE 512
#    define EBD_BLK_CYCLE 10000
#    define EBD_FS_OST_ADR 0x3C000
#    define EBD_FS_START_ADR (EBD_BASE_ADR + EBD_FS_OST_ADR)
#    define EBD_FS_END_ADR (EBD_BASE_ADR + EBD_SIZE)
#    define EBD_FS_BLK_CNT ((EBD_SIZE - EBD_FS_OST_ADR) / EBD_BLK_SIZE)
#    define EBD_FS_CACHE_SIZE 64
#endif  // EBD_FS_C

/********** 类型定义 **********/

typedef struct lfs_config lfs_cfg;
typedef struct lfs        lfs;

/********** 导出的函数 **********/

/**
 * @brief 初始化片内文件系统。
 * @param
 * @retval
 * @warning 必须先初始化模块后才能进行后续操作。
 * @note
 */
int ebd_fs_init(void);

/**
 * @brief 获取片内文件系统实例。
 * @param
 * @retval
 * @warning
 * @note
 */
lfs * ebd_fs_hdl(void);

/**
 * @brief 片内读取。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据读取缓冲区。
 * @param size 读取的字节数。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size);

/**
 * @brief 片内擦除。
 * @param cfg 文件系统实例。
 * @param blk 操作的块。
 * @retval 请参考lfs_error枚举值。。
 * @warning
 * @note
 */
int ebd_fs_erase(const lfs_cfg * cfg, lfs_block_t blk);

/**
 * @brief 片内写入。
 * @param cfg 文件系统实例。
 * @param blk 操作的起始块。
 * @param ofs 块的偏移地址。
 * @param buf 数据写入缓冲区。
 * @param size 写入的字节数。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size);

/**
 * @brief 将缓冲的数据同步写入到片内。
 * @param cfg 文件系统实例。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
int ebd_fs_sync(const lfs_cfg * cfg);

#endif  // EBD_FS_H
