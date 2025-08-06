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
