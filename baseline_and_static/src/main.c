#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"
#include "test_tasks.h"

#ifdef CONFIG_STATIC
#include "mpu_static.h"
#endif

int main(void)
{
    uart_init();
    benchmark_init();
    
    uart_print("\n");
    uart_print("=====================================\n");

#ifdef CONFIG_BASELINE
    uart_print("  Baseline Configuration (No MPU)   \n");
#elif defined(CONFIG_STATIC)
    uart_print("  Static MPU Configuration          \n");
#endif

    uart_print("=====================================\n");
    uart_print("CPU: ARM Cortex-M4 @ 50MHz\n");
    uart_print("RTOS: FreeRTOS\n");
    uart_print("Platform: QEMU MPS2-AN386\n");
    uart_print("=====================================\n\n");

#ifdef CONFIG_STATIC
    mpu_static_init();
    mpu_static_enable();
    mpu_static_print_config();
    uart_print("\n");
#endif

    test_create_tasks();
    
    uart_print("Starting scheduler...\n\n");
    vTaskStartScheduler();

    uart_print("ERROR: Scheduler returned!\n");
    for (;;) {}
}
