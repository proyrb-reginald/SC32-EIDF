#define EBD_FS_C

#include "ebd_fs.h"
#include "sc32_conf.h"
#include "log.h"
#include "fs.h"
#include "user.h"

static lfs_cfg ebd_fs_cfg = {
    .read  = ebd_fs_read,
    .erase = ebd_fs_erase,
    .prog  = ebd_fs_write,
    .sync  = ebd_fs_sync,

    .read_size      = 4,
    .prog_size      = 4,
    .block_size     = EBD_BLK_SIZE,
    .block_count    = EBD_FS_BLK_CNT,
    .block_cycles   = EBD_BLK_CYCLE,
    .cache_size     = EBD_FS_CACHE_SIZE,
    .lookahead_size = (EBD_FS_BLK_CNT >> 3),
};

static lfs ebd_fs;

int ebd_fs_init(void) {
    /********** 挂载文件系统 **********/

    int rst = lfs_mount(&ebd_fs, &ebd_fs_cfg);

    if (rst) {
        OS_PRTF(ERRO_LOG, "mount ebd_fs fail!");
        rst = lfs_format(&ebd_fs, &ebd_fs_cfg);

        if (rst) {
            OS_PRTF(ERRO_LOG, "fmt ebd_fs fail!");
            return rst;
        } else {
            OS_PRTF(NEWS_LOG, "fmt ebd_fs done!");
        }

        rst = lfs_mount(&ebd_fs, &ebd_fs_cfg);
        if (rst) {
            OS_PRTF(ERRO_LOG, "mount ebd_fs fail again!");
            return rst;
        }
    }

    OS_PRTF(NEWS_LOG, "mount ebd_fs done!");

    /********** 更新启动信息 **********/

#if RST_BOOT_INFO
    rst = fs_reset_boot_cnt(&ebd_fs);
    if (rst) {
        OS_PRTF(ERRO_LOG, "rst boot fail with %d!", rst);
    }
#endif
    rst = fs_update_boot_cnt(&ebd_fs);
    rst = fs_get_boot_cnt(&ebd_fs);
    if (rst > -1) {
        OS_PRTF(NEWS_LOG, "boot cnt: %d!", rst);
    }

    /********** 输出文件系统内容 **********/

    rst = fs_ls_dir(&ebd_fs, "/");
    rst = fs_ls_dir(&ebd_fs, SYS_DIR);

    return pdPASS;
}

lfs * ebd_fs_hdl(void) {
    return &ebd_fs;
}

int ebd_fs_read(const lfs_cfg * cfg,
                lfs_block_t     blk,
                lfs_off_t       ofs,
                void *          buf,
                lfs_size_t      size) {
    uint32_t adr = EBD_FS_START_ADR + (blk * EBD_BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    OS_PRTF(INFO_LOG, "read: blk: %u ofs: %u size: %u adr: 0x%x", blk, ofs, size, adr);

    if (IAP_ReadWordArray(adr, buf, cnt) < cnt) {
        OS_PRTF(ERRO_LOG, "read error!");
        return LFS_ERR_IO;
    }

    PRTF(TRAC_LOG, "read buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF(TRAC_LOG, "\n");

    return LFS_ERR_OK;
}

int ebd_fs_erase(const lfs_cfg * cfg, lfs_block_t blk) {
    OS_PRTF(INFO_LOG, "erase: blk: %u", blk);

    IAP_Unlock();
    IAP_EraseSector((EBD_FS_OST_ADR / EBD_BLK_SIZE) + blk);
    IAP_Lock();

    return LFS_ERR_OK;
}

int ebd_fs_write(const lfs_cfg * cfg,
                 lfs_block_t     blk,
                 lfs_off_t       ofs,
                 const void *    buf,
                 lfs_size_t      size) {
    uint32_t adr = EBD_FS_START_ADR + (blk * EBD_BLK_SIZE) + ofs;
    uint32_t cnt = size >> 2;

    OS_PRTF(INFO_LOG, "write: blk: %u ofs: %u size: %u adr: 0x%x", blk, ofs, size, adr);

    PRTF(TRAC_LOG, "write buf: ");
    for (uint32_t i = 0; i < size; i++) {
        PRTF(TRAC_LOG, "0x%02x ", ((uint8_t *)buf)[i]);
    }
    PRTF(TRAC_LOG, "\n");

    IAP_Unlock();
    IAP_WriteCmd(ENABLE);
    if (IAP_ProgramWordArray(adr, (uint32_t *)buf, cnt) < cnt) {
        OS_PRTF(ERRO_LOG, "write error!");
        IAP_Lock();
        return LFS_ERR_IO;
    }
    IAP_Lock();

    return LFS_ERR_OK;
}

int ebd_fs_sync(const lfs_cfg * cfg) {
    return LFS_ERR_OK;
}
