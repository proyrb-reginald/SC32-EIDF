#include "os.h"
#include "log.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>

#if (SYS_TASK_CNT > 16)
#    warning "system task list is too large!"
#endif

SemaphoreHandle_t os_md_init_done = NULL;

SemaphoreHandle_t dma_mutex = NULL;

static TaskHandle_t sys_task[SYS_TASK_CNT] = {NULL};

void vApplicationDaemonTaskStartupHook(void) {
    /********** 初始化系统资源 **********/

    BaseType_t rst = os_init();
    if (rst != pdPASS) {
        configASSERT(0);
    }

    /********** 创建主任务 **********/

    rst = xTaskCreate(os_task_init, "os_task_init", 80, 0, configMAX_PRIORITIES - 3, 0);

    if (rst != pdPASS) {
        OS_PRTF(ERRO_LOG, "create os_task_init fail!");
        configASSERT(0);
    }

    /********** 创建模块初始化任务 **********/

    rst = xTaskCreate(os_md_init, "os_md_init", 360, 0, configMAX_PRIORITIES - 3, 0);

    if (rst != pdPASS) {
        OS_PRTF(ERRO_LOG, "create os_md_init fail!");
        configASSERT(0);
    }
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

        TaskHandle_t sys_task_hdl = xTimerGetTimerDaemonTaskHandle();
        PRTF(NEWS_LOG, " [%c] %s: %zu W\n", get_task_state(sys_task_hdl),
             pcTaskGetName(sys_task_hdl), uxTaskGetStackHighWaterMark2(sys_task_hdl));

        sys_task_hdl = xTaskGetIdleTaskHandle();
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
