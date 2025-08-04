#include "init.h"

void rcc_init(void) {
    if (RCC_Unlock(0xFF) != SUCCESS) {
        while (1) {}
    }

    RCC_ITConfig(DISABLE);
    RCC_HXTCmd(DISABLE);
    RCC_LXTCmd(DISABLE);
    RCC_HIRCCmd(ENABLE);
    RCC_LIRCCmd(DISABLE);
    RCC_HIRCDIV1Cmd(ENABLE);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    if (RCC_SYSCLKConfig(RCC_SYSCLKSource_HIRC) != SUCCESS) {
        while (1) {}
    }

    RCC_APB0Config(RCC_HCLK_Div1);
    RCC_APB0Cmd(ENABLE);

    uart_5_init();

    printf("switch system clock to HIRC done!\n");
}

void gpio_init(void) {
    // lcd驱动引脚配置
    GPIO_InitTypeDef GPIOInit_PC12_Struct;  // 复位引脚
    GPIOInit_PC12_Struct.GPIO_Pin        = GPIO_Pin_12;
    GPIOInit_PC12_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC12_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC12_Struct);
    GPIO_InitTypeDef GPIOInit_PC13_Struct;  // 数据命令引脚
    GPIOInit_PC13_Struct.GPIO_Pin        = GPIO_Pin_13;
    GPIOInit_PC13_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC13_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC13_Struct);
    GPIO_InitTypeDef GPIOInit_PC14_Struct;  // 片选引脚
    GPIOInit_PC14_Struct.GPIO_Pin        = GPIO_Pin_14;
    GPIOInit_PC14_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PC14_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOC, &GPIOInit_PC14_Struct);

    // w25q64引脚配置
    GPIO_InitTypeDef GPIOInit_PB13_Struct;  // 片选引脚
    GPIOInit_PB13_Struct.GPIO_Pin        = GPIO_Pin_13;
    GPIOInit_PB13_Struct.GPIO_Mode       = GPIO_Mode_OUT_PP;
    GPIOInit_PB13_Struct.GPIO_DriveLevel = 0;
    GPIO_Init(GPIOB, &GPIOInit_PB13_Struct);
}

void uart_5_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_UART5, ENABLE);  // 使能时钟
    UART_InitTypeDef UART_InitStruct;                      // 初始化结构体
    UART_InitStruct.UART_ClockFrequency = 64000000;        // 设置时钟频率
    UART_InitStruct.UART_BaudRate       = 115200;          // 设置波特率为115200
    UART_InitStruct.UART_Mode           = UART_Mode_10B;   // 设置工作模式为10位数据位
    UART_Init(UART5, &UART_InitStruct);                    // 初始化
    UART_ITConfig(UART5, UART_IT_EN, DISABLE);             // 禁用中断
    UART_PinRemapConfig(UART5, UART_PinRemap_Default);     // 设置引脚复用为默认配置
    UART_TXCmd(UART5, ENABLE);                             // 使能发送功能
    UART_RXCmd(UART5, ENABLE);                             // 使能接收功能
    Printf_UartInit(UART5);                                // 设置为Printf的输出设备
}

void spi_2_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI2, ENABLE);  // 使能时钟
    SPI_InitTypeDef SPI_InitStruct;                        // 初始化结构体
    SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;        // 设置工作模式为主模式
    SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8B;        // 设置数据大小为8位
    SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;         // 设置时钟相位为第一个边沿采样
    SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;           // 设置时钟极性为低电平
    SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;       // 设置数据传输的首位为最高位
    SPI_InitStruct.SPI_Prescaler = SPI_Prescaler_4;        // 设置预分频为4
    SPI_Init(SPI2, &SPI_InitStruct);                       // 初始化
    SPI_PinRemapConfig(SPI2, SPI_PinRemap_C);              // 设置引脚映射
    SPI_ITConfig(SPI2, SPI_IT_INTEN, DISABLE);             // 禁用中断
    SPI_DMACmd(SPI2, SPI_DMAReq_TX, DISABLE);              // 关闭发送DMA请求
    SPI_DMACmd(SPI2, SPI_DMAReq_RX, DISABLE);              // 关闭发送DMA请求
    SPI_Cmd(SPI2, ENABLE);                                 // 使能
}

void qspi_0_init(void) {
    RCC_APB0PeriphClockCmd(RCC_APB0Periph_QTWI0, ENABLE);  // 使能时钟
    QSPI_InitTypeDef QSPI_InitStruct;                      // 初始化结构体
    QSPI_InitStruct.QSPI_SShift    = QSPI_SShift_OFF;      // 禁止延迟采样
    QSPI_InitStruct.QSPI_Prescaler = QSPI_Prescaler_4;     // 设置预分频
    QSPI_InitStruct.QSPI_DWidth    = QSPI_DWidth_8bit;     // 设置数据宽度
    QSPI_InitStruct.QSP_LMode      = QSPI_LMode_1Line;     // 设置单线模式
    QSPI_InitStruct.QSPI_Mode      = QSPI_Mode_QSPI;       // 设置半双工
    QSPI_InitStruct.QSPI_CPMode    = QSPI_CPMode_Low;      // 设置时钟极性为低
    QSPI_InitStruct.QSPI_RW        = QSPI_RW_Write;        // 设置为写模式
    QSPI_InitStruct.QSPI_CLKONLY   = QSPI_CLKONLY_OFF;     // 禁止只输出时钟
    QSPI_Init(QSPI0, &QSPI_InitStruct);                    // 初始化
    QSPI_ITConfig(QSPI0, QSPI_IT_INTEN, DISABLE);          // 禁用总中断
    QSPI_DMACmd(QSPI0, QSPI_DMAReq_TX, DISABLE);           // 关闭DMA发送请求
    QSPI_DMACmd(QSPI0, QSPI_DMAReq_RX, DISABLE);           // 关闭DMA接收请求
    QSPI_Cmd(QSPI0, ENABLE);                               // 使能
}

void dma_0_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_HIGH;         // 设置优先级为高
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字节
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_FIXED;      // 设置目标地址固定
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_INC;        // 设置源地址循环递增
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_SPI2_TX;   // 设置请求源为SPI2发送
    DMA_InitStruct.DMA_SrcAddress   = 0;                         // 设置源地址为0
    DMA_InitStruct.DMA_DstAddress =
        (uint32_t)&(SPI2->SPI_DATA);             // 设置目标地址为SPI2数据寄存器
    DMA_Init(DMA0, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA0, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA0, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA0, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA0, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA0, DMA_DMAReq_CHRQ, DISABLE);  // 关闭的DMA请求
    __NVIC_SetPriority(DMA0_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA0_IRQn);                 // 使能中断
    DMA_Cmd(DMA0, DISABLE);                      // 先关闭使能
}

void dma_1_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_MEDIUM;       // 设置优先级为中
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字节
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_FIXED;      // 设置目标地址固定
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_INC;        // 设置源地址循环递增
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_QSPI0_TX;  // 设置请求源为QSPI0发送
    DMA_InitStruct.DMA_SrcAddress   = 0;                         // 设置源地址为0
    DMA_InitStruct.DMA_DstAddress =
        (uint32_t)&(QSPI0->QSPI_DATA);           // 设置目标地址为QSPI数据寄存器
    DMA_Init(DMA1, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA1, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA1, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA1, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA1, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA1, DMA_DMAReq_CHRQ, DISABLE);  // 关闭DMA请求
    __NVIC_SetPriority(DMA1_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA1_IRQn);                 // 使能中断
    DMA_Cmd(DMA1, DISABLE);                      // 先关闭使能
}

void dma_2_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);            // 使能时钟
    DMA_InitTypeDef DMA_InitStruct;                              // 初始化结构体
    DMA_InitStruct.DMA_Priority     = DMA_Priority_MEDIUM;       // 设置优先级为中
    DMA_InitStruct.DMA_CircularMode = DMA_CircularMode_Disable;  // 禁用循环模式
    DMA_InitStruct.DMA_DataSize     = DMA_DataSize_Byte;         // 设置数据大小为字
    DMA_InitStruct.DMA_TargetMode   = DMA_TargetMode_INC;        // 设置目标地址递增
    DMA_InitStruct.DMA_SourceMode   = DMA_SourceMode_FIXED;      // 设置源地址固定
    DMA_InitStruct.DMA_Burst        = DMA_Burst_Disable;         // 禁用突发传输
    DMA_InitStruct.DMA_BufferSize   = 0;                         // 设置缓冲区大小为0
    DMA_InitStruct.DMA_Request      = DMA_Request_TWI_QSPI0_RX;  // 设置请求源为QSPI0接收
    DMA_InitStruct.DMA_SrcAddress =
        (uint32_t)&(QSPI0->QSPI_DATA);           // 设置源地址为QSPI数据寄存器
    DMA_InitStruct.DMA_DstAddress = 0;           // 设置目标地址为0
    DMA_Init(DMA2, &DMA_InitStruct);             // 初始化
    DMA_ITConfig(DMA2, DMA_IT_INTEN, ENABLE);    // 使能总中断
    DMA_ITConfig(DMA2, DMA_IT_TCIE, ENABLE);     // 使能传输完成中断
    DMA_ITConfig(DMA2, DMA_IT_HTIE, DISABLE);    // 禁用半传输中断
    DMA_ITConfig(DMA2, DMA_IT_TEIE, DISABLE);    // 禁用传输错误中断
    DMA_DMACmd(DMA2, DMA_DMAReq_CHRQ, DISABLE);  // 关闭DMA请求
    __NVIC_SetPriority(DMA2_IRQn, 1);            // 设置中断优先级为1
    __NVIC_EnableIRQ(DMA2_IRQn);                 // 使能中断
    DMA_Cmd(DMA2, DISABLE);                      // 先关闭使能
}

void bsp_init(void) {
    rcc_init();
    gpio_init();
    spi_2_init();
    qspi_0_init();
    dma_0_init();
    dma_1_init();
    dma_2_init();
    printf("init bsp done!\n");
}
