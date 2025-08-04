#include "user.h"
#include "os.h"
#include "log.h"
#include "st7789v.h"
#include "w25q64.h"
#include "ebd_fs.h"
#include "etn_fs.h"
#include "gui.h"

void os_md_init(void * task_arg) {
    /********** 初始化驱动 **********/

    BaseType_t rst;

#if ADD_W25Q64
    rst = w25q64_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }
#endif  // ADD_W25Q64

#if ADD_ST7789V
    rst = st7789v_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }
#endif  // ADD_ST7789V

    /********** 初始化组件 **********/

#if ADD_EBD_FS
    rst = ebd_fs_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }
#endif  // ADD_EBD_FS

#if ADD_ETN_FS
    rst = etn_fs_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }
#endif  // ADD_ETN_FS

#if ADD_GUI
    rst = gui_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }
#endif  // ADD_GUI

    OS_MD_INIT_DONE();
}

void os_task_init(void * task_arg) {
    /********** 等待模块初始化完成 **********/

    WAIT_OS_MD_INIT();

    /********** 创建任务 **********/

    BaseType_t   rst;
    TaskHandle_t task_hdl;

#if ADD_ST7789V
    rst =
        xTaskCreate(st7789v_task, "lcd", 60, NULL, (configMAX_PRIORITIES - 3), &task_hdl);
    if (rst != pdPASS) {
        while (1) {
            OS_PRTF(ERRO_LOG, "create lcd fail!");
            configASSERT(0);
        }
    } else {
        OS_PRTF(NEWS_LOG, "create lcd done!");
        rst = os_add_task(&task_hdl);
        if (rst != pdPASS) {
            configASSERT(0);
        }
    }
#endif  // ADD_ST7789V

#if ADD_GUI
    rst = xTaskCreate(gui_task, "gui", 650, NULL, 1, &task_hdl);
    if (rst != pdPASS) {
        while (1) {
            OS_PRTF(ERRO_LOG, "create gui fail!");
            configASSERT(0);
        }
    } else {
        OS_PRTF(NEWS_LOG, "create gui done!");
        rst = os_add_task(&task_hdl);
        if (rst != pdPASS) {
            configASSERT(0);
        }
    }
#endif  // ADD_GUI

    OS_TASK_INIT_DONE();
}
