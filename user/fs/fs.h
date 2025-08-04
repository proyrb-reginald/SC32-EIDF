#ifndef FS_H
#define FS_H

/**
 * @brief 提供文件系统常用函数
 * @details
 * @file fs.h
 * @author proyrb
 * @date 2025/7/30
 * @note
 */

/********** 导入需要的头文件 **********/

#include "lfs.h"
#include "lvgl.h"

/********** 配置文件系统 **********/

// 一级目录
#define SYS_DIR "/sys/"
#define RES_DIR "/res/"

// 启动文件
#define BOOT_FILE "boot.dat"
#define BOOT_FILE_PATH SYS_DIR "boot.dat"

// 图片资源
#define RES_FILE "sbg.jpg"
#define RES_FILE_PATH RES_DIR "sbg.jpg"

/********** 类型定义 **********/

typedef struct lfs_info lfs_info;

typedef enum lfs_error lfs_error;

/********** 导出的函数 **********/

/**
 * @brief 获取系统启动次数。
 * @param lfs 文件系统实例。
 * @retval 非负数表示启动的次数。
 * @retval -1 表示文件读取数据量有误。
 * @retval 其余请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_get_boot_cnt(lfs_t * const lfs);

/**
 * @brief 更新系统启动次数。
 * @param lfs 文件系统实例。
 * @retval -1 表示文件写入数据量有误。
 * @retval 其余请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_update_boot_cnt(lfs_t * const lfs);

/**
 * @brief 重置系统启动次数。
 * @param lfs 文件系统实例。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error fs_reset_boot_cnt(lfs_t * const lfs);

/**
 * @brief 列出指定目录的内容。
 * @param lfs 文件系统实例。
 * @param dir 指定的目录。
 * @retval 请参考lfs_error枚举值。
 * @warning 非递归。
 * @note
 */
lfs_error fs_ls_dir(lfs_t * const lfs, const char * dir);

/**
 * @brief 向特定目录写入指定的图片数据。
 * @param lfs 文件系统实例。
 * @param res 图片资源数据。
 * @param name 写入后的图片文件的名称。
 * @retval 请参考lfs_error枚举值。
 * @warning
 * @note
 */
lfs_error
fs_write_res(lfs_t * const lfs, const lv_image_dsc_t * const res, const char * name);

#endif  // FS_H
