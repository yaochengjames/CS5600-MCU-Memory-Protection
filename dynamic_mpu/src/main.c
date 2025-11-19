#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"
#include "test_tasks.h"
#include "mpu_dynamic.h"

int main(void)
{
    uart_init();
    benchmark_init();
    
    uart_print("\n");
    uart_print("=====================================\n");
    uart_print("  Dynamic MPU Configuration         \n");
    uart_print("=====================================\n");
    uart_print("CPU: ARM Cortex-M4 @ 50MHz\n");
    uart_print("RTOS: FreeRTOS\n");
    uart_print("Platform: QEMU MPS2-AN386\n");
    uart_print("=====================================\n\n");

    mpu_dynamic_init();
    mpu_dynamic_print_config();
    uart_print("\n");

    test_create_tasks();
    
    uart_print("Starting scheduler...\n");
    uart_print("(MPU will reconfigure on every context switch)\n\n");
    
    vTaskStartScheduler();

    uart_print("ERROR: Scheduler returned!\n");
    for (;;) {}
}
