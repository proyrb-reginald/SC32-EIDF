#include "os.h"
#include "log.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "user.h"
#include "st7789v.h"
#include "gui.h"
#include <stdio.h>

#if (SYS_TASK_CNT > 16)
#    warning "system task list is too large!"
#endif

SemaphoreHandle_t os_md_init_done = NULL;

SemaphoreHandle_t dma_mutex = NULL;

static TaskHandle_t sys_task[SYS_TASK_CNT] = {NULL};

#define GUI_TASK_STACK 1250
static StackType_t  main_task_stack[GUI_TASK_STACK];
static StaticTask_t main_task_tcb;

#define LCD_TASK_STACK 50
static StackType_t  lcd_task_stack[LCD_TASK_STACK];
static StaticTask_t lcd_task_tcb;

static TaskHandle_t tim_task_hdl;

void vApplicationDaemonTaskStartupHook(void) {
    /********** 初始化系统资源 **********/

    BaseType_t init_rst = os_init();
    if (init_rst != pdPASS) {
        configASSERT(0);
    }

    /********** 创建模块初始化任务 **********/

    TaskHandle_t creat_rst =
        xTaskCreateStatic(os_md_init, "os_md_init", 350, NULL, configMAX_PRIORITIES - 2,
                          main_task_stack, &main_task_tcb);
    if (creat_rst == NULL) {
        OS_PRTF(ERRO_LOG, "create os_md_init fail!");
        configASSERT(0);
    }
    if (xSemaphoreTake(os_md_init_done, portMAX_DELAY) == pdTRUE) {
        vTaskDelay(50);
    }

    /********** 创建主任务 **********/

#if ADD_ST7789V
    creat_rst =
        xTaskCreateStatic(st7789v_task, "lcd", LCD_TASK_STACK, NULL,
                          (configMAX_PRIORITIES - 4), lcd_task_stack, &lcd_task_tcb);
    if (init_rst != pdPASS) {
        OS_PRTF(ERRO_LOG, "create lcd fail!");
        configASSERT(0);
    } else {
        OS_PRTF(NEWS_LOG, "create lcd done!");
        init_rst = os_add_task(&creat_rst);
        if (init_rst != pdPASS) {
            configASSERT(0);
        }
    }
#endif  // ADD_ST7789V

#if ADD_GUI
    creat_rst = xTaskCreateStatic(gui_task, "gui", GUI_TASK_STACK, NULL, 1,
                                  main_task_stack, &main_task_tcb);
    if (init_rst != pdPASS) {
        OS_PRTF(ERRO_LOG, "create gui fail!");
        configASSERT(0);
    } else {
        OS_PRTF(NEWS_LOG, "create gui done!");
        init_rst = os_add_task(&creat_rst);
        if (init_rst != pdPASS) {
            configASSERT(0);
        }
    }
#endif  // ADD_GUI

    tim_task_hdl = xTimerGetTimerDaemonTaskHandle();
}

void vApplicationMallocFailedHook(void) {
    OS_PRTF(ERRO_LOG, "malloc fail!");
}

static char get_task_state(TaskHandle_t xTask) {
    switch (eTaskGetState(xTask)) {
        case eRunning: return 'W';
        case eReady: return 'R';
        case eBlocked: return 'B';
        case eSuspended: return 'S';
        case eDeleted: return 'D';
        default: return 'U';
    }
}

void vApplicationIdleHook(void) {
    static TickType_t last_time = 0;
    TickType_t        now_time  = xTaskGetTickCount();

    if ((now_time - last_time) >= pdMS_TO_TICKS(MNT_CALL_PERIOD)) {
        OS_PRTF(NEWS_LOG, "free mem:");
        PRTF(NEWS_LOG, " heap: %u/%u B\n", xPortGetFreeHeapSize(),
             xPortGetMinimumEverFreeHeapSize());
        for (uint8_t i = 0; i < SYS_TASK_CNT; i++) {
            if (sys_task[i]) {
                PRTF(NEWS_LOG, " [%c] %s: %zu W\n", get_task_state(sys_task[i]),
                     pcTaskGetName(sys_task[i]),
                     uxTaskGetStackHighWaterMark2(sys_task[i]));
            }
        }

        PRTF(NEWS_LOG, " [%c] %s: %zu W\n", get_task_state(tim_task_hdl),
             pcTaskGetName(tim_task_hdl), uxTaskGetStackHighWaterMark2(tim_task_hdl));

        TaskHandle_t sys_task_hdl = xTaskGetCurrentTaskHandle();
        PRTF(NEWS_LOG, " [%c] %s: %zu W\n", get_task_state(sys_task_hdl),
             pcTaskGetName(sys_task_hdl), uxTaskGetStackHighWaterMark2(sys_task_hdl));

        last_time = now_time;
    }
}

BaseType_t os_init(void) {
    if (os_md_init_done == NULL) {
        os_md_init_done = xSemaphoreCreateBinary();
        if (os_md_init_done == NULL) {
            OS_PRTF(ERRO_LOG, "create os_md_init_done fail!\n");
            return pdFAIL;
        }
    }

    if (dma_mutex == NULL) {
        dma_mutex = xSemaphoreCreateMutex();
        if (dma_mutex == NULL) {
            OS_PRTF(ERRO_LOG, "create dma_mutex fail!\n");
            return pdFAIL;
        }
    }

    return pdPASS;
}

BaseType_t os_add_task(TaskHandle_t * const task_hdl) {
    for (uint8_t i = 0; i < SYS_TASK_CNT; i++) {
        if (sys_task[i] == NULL) {
            sys_task[i] = *task_hdl;
            return pdPASS;
        }
    }
    OS_PRTF(ERRO_LOG, "add task to os fail!\n");
    return pdFAIL;
}
