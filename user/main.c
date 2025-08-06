#include "main.h"
#include "init.h"
#include "log.h"
#include "st7789v.h"
#include "w25q64.h"
#include "ebd_fs.h"
#include "etn_fs.h"
#include "gui.h"

__attribute__((noreturn)) int main(void) {
    /********** 初始化片上资源 **********/

    bsp_init();
    OS_PRTF(NEWS_LOG, "init mid done!");

    /********** 初始化驱动 **********/

    int rst;

#if ADD_W25Q64
    rst = w25q64_init();
    configASSERT(rst);
#endif  // ADD_W25Q64

#if ADD_ST7789V
    rst = st7789v_init();
    configASSERT(rst);
#endif  // ADD_ST7789V

    /********** 初始化组件 **********/

#if ADD_EBD_FS
    rst = ebd_fs_init();
    configASSERT(rst);
#endif  // ADD_EBD_FS

#if ADD_ETN_FS
    rst = etn_fs_init();
    configASSERT(rst);
#endif  // ADD_ETN_FS

#if ADD_GUI
    gui_init();
#endif  // ADD_GUI

    while (1) {
        OS_PRTF(WARN_LOG, "sys end!");
    }
}
