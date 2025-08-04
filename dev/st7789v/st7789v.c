#define ST7789V_C

#include "st7789v.h"
#include "sc32_conf.h"
#include "os.h"
#include "log.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static SemaphoreHandle_t trig_async_dma = NULL;
static SemaphoreHandle_t dma_done       = NULL;
static st7789v_arg       dma;
static uint8_t           clm_data[4], row_data[4];
static st7789v_arg       clm_arg, row_arg;

BaseType_t st7789v_init(void) {
    if (trig_async_dma == NULL) {
        trig_async_dma = xSemaphoreCreateBinary();
        if (trig_async_dma == NULL) {
            OS_PRTF(ERRO_LOG, "create trig_async_dma fail!\n");
            return pdFAIL;
        }
    }

    if (dma_done == NULL) {
        dma_done = xSemaphoreCreateBinary();
        if (dma_done == NULL) {
            OS_PRTF(ERRO_LOG, "create dma_done fail!\n");
            return pdFAIL;
        }
    }

    clm_arg.data = clm_data;
    clm_arg.size = sizeof(clm_data);
    row_arg.data = row_data;
    row_arg.size = sizeof(row_data);

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    vTaskDelay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 0);
    vTaskDelay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    vTaskDelay(120);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    OS_PRTF(INFO_LOG, "reset done!");

    uint8_t     data[2];
    st7789v_arg arg;

    st7789v_ctl(Wake, NULL);
    vTaskDelay(120);
    OS_PRTF(INFO_LOG, "wake done!");

    data[0]  = 0x0;
    arg.data = data;
    arg.size = 1;
    st7789v_ctl(SetRAMReadMode, &arg);
    OS_PRTF(INFO_LOG, "set read mode done!");

    data[0]  = 0x55;
    arg.data = data;
    arg.size = 1;
    st7789v_ctl(SetColorFmt, &arg);
    OS_PRTF(INFO_LOG, "set color fmt done!");

    data[0]  = 0x00;
    data[1]  = 0x08;
    arg.data = data;
    arg.size = 2;
    st7789v_ctl(SetRGB, &arg);
    OS_PRTF(INFO_LOG, "set rgb fmt done!");

    st7789v_ctl(OnReverse, NULL);
    OS_PRTF(INFO_LOG, "reverse done!");

    st7789v_ctl(OnDisplay, NULL);
    vTaskDelay(50);

    OS_PRTF(NEWS_LOG, "init done!");

    return pdPASS;
}

__inline__ void st7789v_dma_irq(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(dma_done, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

__attribute__((optnone)) void st7789v_ctl(const st7789v_cmd         cmd,
                                          const st7789v_arg * const arg) {
    GPIO_WriteBit(MODE_GPIO_GRP, MODE_GPIO_PIN, 0);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 0);

    SPI_SendData(USE_SPI, cmd);
    while (SPI_GetFlagStatus(USE_SPI, SPI_FLAG_QTWIF) == 0) {}

    if ((arg != NULL) && (arg->size > 0)) {
        GPIO_WriteBit(MODE_GPIO_GRP, MODE_GPIO_PIN, 1);
        switch (cmd) {
            case Write: {
                if (xSemaphoreTake(dma_mutex, portMAX_DELAY) == pdTRUE) {
                    DMA_SetSrcAddress(USE_DMA, (uint32_t)dma.data);
                    DMA_SetCurrDataCounter(USE_DMA, dma.size);
                    DMA_Cmd(USE_DMA, ENABLE);
                    SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, ENABLE);
                    DMA_SoftwareTrigger(USE_DMA);

                    if (xSemaphoreTake(dma_done, portMAX_DELAY) == pdTRUE) {
                        SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, DISABLE);
                        DMA_Cmd(USE_DMA, DISABLE);
                    }

                    LVGL_DONE();
                    xSemaphoreGive(dma_mutex);
                }
                break;
            }
            default: {
                for (uint8_t i = 0; i < arg->size; ++i) {
                    SPI_SendData(USE_SPI, arg->data[i]);
                    while (SPI_GetFlagStatus(USE_SPI, SPI_FLAG_QTWIF) == 0) {}
                }
                break;
            }
        }
    }

    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
}

#if TEST
#    include <string.h>
#    define LCD_WIDTH 240
#    define LCD_HIGHT 320
#    define TEST_SIZE 4
#    define FLUSH_CNT (LCD_HIGHT / TEST_SIZE)
static uint8_t                        test_data_r[LCD_WIDTH * TEST_SIZE * 2] = {0};
static uint8_t                        test_data_b[LCD_WIDTH * TEST_SIZE * 2] = {0};
__attribute__((noreturn)) static void test_task(void * task_arg) {
    for (uint16_t i = 0; i < sizeof(test_data_r); i++) {
        if (i % 2 == 0) {
            test_data_r[i] = 0x00;
            test_data_b[i] = 0x1F;
        } else {
            test_data_r[i] = 0xF8;
            test_data_b[i] = 0x00;
        }
    }

    uint8_t  flag = 0;
    uint16_t s_x = 0, e_x = LCD_WIDTH - 1, s_y, e_y;
    while (1) {
        for (uint8_t i = 0; i < FLUSH_CNT; i++) {
            s_y = i * TEST_SIZE;
            e_y = s_y + TEST_SIZE - 1;
            if (flag) {
                st7789v_async_fill(s_x, e_x, s_y, e_y, test_data_b, sizeof(test_data_b));
            } else {
                st7789v_async_fill(s_x, e_x, s_y, e_y, test_data_r, sizeof(test_data_r));
            }
        }
        flag = (flag) ? 0 : 1;
        vTaskDelay(100);
    }
}
#endif  // TEST

__attribute__((noreturn)) void st7789v_task(void * task_arg) {
#if TEST
    TaskHandle_t task_hdl;
    xTaskCreate(test_task, "test", 70, NULL, 2, &task_hdl);
    os_add_task(&task_hdl);
#endif  // TEST

    while (1) {
        if (xSemaphoreTake(trig_async_dma, portMAX_DELAY) == pdTRUE) {
            st7789v_ctl(SetColumn, &clm_arg);
            st7789v_ctl(SetRow, &row_arg);
            st7789v_ctl(Write, &dma);
        }
    }
}

void st7789v_async_fill(uint16_t           s_x,
                        uint16_t           e_x,
                        uint16_t           s_y,
                        uint16_t           e_y,
                        const void * const buf,
                        const uint32_t     size) {
    clm_data[0] = s_x >> 8;
    clm_data[1] = s_x;
    clm_data[2] = e_x >> 8;
    clm_data[3] = e_x;

    row_data[0] = s_y >> 8;
    row_data[1] = s_y;
    row_data[2] = e_y >> 8;
    row_data[3] = e_y;

    dma.data = (uint8_t *)buf;
    dma.size = size;

    xSemaphoreGive(trig_async_dma);
}
