#define ST7789V_C

#include "st7789v.h"
#include "sc32_conf.h"
#include "log.h"
#include <stdbool.h>

#if ST7789V_TEST
static volatile bool dma_done = false;
#endif  // ST7789V_TEST

int8_t st7789v_init(void) {
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    SC_Delay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 0);
    SC_Delay(1);
    GPIO_WriteBit(RESET_GPIO_GRP, RESET_GPIO_PIN, 1);
    SC_Delay(120);
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
    OS_PRTF(INFO_LOG, "reset done!");

    uint8_t     data[2];
    st7789v_arg arg;

    st7789v_ctl(Wake, NULL);
    SC_Delay(120);
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
    SC_Delay(50);

    OS_PRTF(NEWS_LOG, "init done!");

#if ST7789V_TEST
    st7789v_test();
#endif  // ST7789V_TEST

    return 0;
}

__inline__ void st7789v_dma_irq(void) {
    SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, DISABLE);
    DMA_Cmd(USE_DMA, DISABLE);
    LVGL_DONE();
    GPIO_WriteBit(CHIP_GPIO_GRP, CHIP_GPIO_PIN, 1);
#if ST7789V_TEST
    dma_done = true;
#endif  // ST7789V_TEST
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
                DMA_SetSrcAddress(USE_DMA, (uint32_t)arg->data);
                DMA_SetCurrDataCounter(USE_DMA, arg->size);
                DMA_Cmd(USE_DMA, ENABLE);
                SPI_DMACmd(USE_SPI, SPI_DMAReq_TX, ENABLE);
                DMA_SoftwareTrigger(USE_DMA);
                return;
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

#if ST7789V_TEST
#    include <string.h>
#    define LCD_WIDTH 240
#    define LCD_HIGHT 320
#    define TEST_SIZE 4
#    define FLUSH_CNT (LCD_HIGHT / TEST_SIZE)
static uint8_t                 test_data_r[LCD_WIDTH * TEST_SIZE * 2] = {0};
static uint8_t                 test_data_b[LCD_WIDTH * TEST_SIZE * 2] = {0};
__attribute__((noreturn)) void st7789v_test(void) {
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
            while (!dma_done) {}
            dma_done = false;
        }
        flag = (flag) ? 0 : 1;
        SC_Delay(100);
    }
}
#endif  // ST7789V_TEST

void st7789v_async_fill(uint16_t           s_x,
                        uint16_t           e_x,
                        uint16_t           s_y,
                        uint16_t           e_y,
                        const void * const buf,
                        const uint32_t     size) {
    uint8_t     data[4];
    st7789v_arg arg = {
        .data = data,
        .size = sizeof(data),
    };

    data[0] = s_x >> 8;
    data[1] = s_x;
    data[2] = e_x >> 8;
    data[3] = e_x;
    st7789v_ctl(SetColumn, &arg);

    data[0] = s_y >> 8;
    data[1] = s_y;
    data[2] = e_y >> 8;
    data[3] = e_y;
    st7789v_ctl(SetRow, &arg);

    arg.data = (uint8_t *)buf;
    arg.size = size;
    st7789v_ctl(Write, &arg);
}
