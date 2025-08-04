#include "irq.h"
#include "st7789v.h"
#include "w25q64.h"

__attribute__((interrupt("IRQ"))) void DMA0_IRQHandler(void) {
    DMA_ClearFlag(DMA0, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    st7789v_dma_irq();
}

__attribute__((interrupt("IRQ"))) void DMA1_IRQHandler(void) {
    DMA_ClearFlag(DMA1, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    w25q64_dma_irq();
}

__attribute__((interrupt("IRQ"))) void DMA2_IRQHandler(void) {
    DMA_ClearFlag(DMA2, DMA_FLAG_GIF | DMA_FLAG_TCIF | DMA_FLAG_HTIF | DMA_FLAG_TEIF);
    w25q64_dma_irq();
}
