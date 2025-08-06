#include "irq.h"
#include "st7789v.h"
#include "w25q64.h"

void SysTick_Handler(void) {
    SC_IncTick();
    lv_tick_inc(1);
}

__attribute__((interrupt("IRQ"))) void DMA0_IRQHandler(void) {
    DMA_ClearFlag(DMA0, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    st7789v_dma_irq();
}

__attribute__((interrupt("IRQ"))) void DMA1_IRQHandler(void) {
    DMA_ClearFlag(DMA1, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    w25q64_dma_tx_irq();
}

__attribute__((interrupt("IRQ"))) void DMA2_IRQHandler(void) {
    DMA_ClearFlag(DMA2, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    w25q64_dma_rx_irq();
}
