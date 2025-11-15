#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"
#include "test_tasks.h"

int main(void)
{
    uart_init();
    benchmark_init();
    
    uart_print("\n");
    uart_print("=====================================\n");
    uart_print("  Baseline Configuration (No MPU)   \n");
    uart_print("=====================================\n");
    uart_print("CPU: ARM Cortex-M4 @ 50MHz\n");
    uart_print("RTOS: FreeRTOS\n");
    uart_print("Platform: QEMU MPS2-AN386\n");
    uart_print("=====================================\n\n");

    test_create_tasks();
    
    uart_print("Starting scheduler...\n\n");
    vTaskStartScheduler();

    // Should never reach here
    uart_print("ERROR: Scheduler returned!\n");
    for (;;) {}
}
