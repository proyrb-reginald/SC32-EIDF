#include "main.h"
#include "init.h"
#include "log.h"
#include "os.h"

__attribute__((noreturn)) int main(void) {
    /********** 初始化片上资源 **********/

    bsp_init();
    printf("init mid done!\n");

    /********** 启动调度器 **********/

    vTaskStartScheduler();
    printf("start os fail!\n");

    while (1) {}
}
