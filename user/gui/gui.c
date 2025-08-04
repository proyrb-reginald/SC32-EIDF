#include "gui.h"
#include "os.h"
#include "log.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "etn_fs.h"
#include "ui.h"
#include "main.h"

BaseType_t gui_init(void) {
    lv_init();
    lv_tick_set_cb(xTaskGetTickCount);
#if ADD_ETN_FS
    lv_littlefs_set_handler(etn_fs_hdl());
#endif
    lv_port_disp_init();
    ui_init();
    OS_PRTF(NEWS_LOG, "init lvgl done!");

    return pdPASS;
}

__attribute__((noreturn)) void gui_task(void * task_arg) {
    while (1) {
        uint32_t slp_tm = lv_timer_handler();
        vTaskDelay((slp_tm > GUI_MIN_SLP_MS) ? slp_tm : GUI_MIN_SLP_MS);
    }
}
