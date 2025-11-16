#include "test_tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"

#define NUM_SAMPLES 1000
#define SYSTICK_MAX 0x00FFFFFF

static volatile uint32_t switch_count = 0;
static volatile uint64_t total_cycles = 0;
static volatile uint32_t min_cycles = 0xFFFFFFFF;
static volatile uint32_t max_cycles = 0;

#ifdef CONFIG_BASELINE
    static const char* config_name = "Baseline";
#elif defined(CONFIG_STATIC)
    static const char* config_name = "Static";
#elif defined(CONFIG_DYNAMIC)
    static const char* config_name = "Dynamic";
#else
    static const char* config_name = "Unknown";
#endif

void TaskA(void *pvParameters)
{
    (void)pvParameters;
    uint32_t current, elapsed;
    
    for (;;) {
        current = benchmark_get_cycles();
        
        vTaskDelay(pdMS_TO_TICKS(10));
        
        uint32_t after = benchmark_get_cycles();
        
        if (after >= current) {
            elapsed = after - current;
        } else {
            elapsed = (SYSTICK_MAX - current) + after;
        }
        
        if (elapsed > 0) {
            total_cycles += elapsed;
            switch_count++;
            
            if (elapsed < min_cycles) min_cycles = elapsed;
            if (elapsed > max_cycles) max_cycles = elapsed;
        }
        
        if (switch_count > 0 && switch_count % 100 == 0) {
            uart_print("[");
            uart_print(config_name);
            uart_print("] Switches: ");
            uart_printf("%lu", switch_count);
            uart_print(", Avg: ");
            uart_printf("%lu", (uint32_t)(total_cycles / switch_count));
            uart_print(" cycles\n");
        }
        
        if (switch_count >= NUM_SAMPLES) {
            test_print_statistics();
            uart_print("\n=== ");
            uart_print(config_name);
            uart_print(" measurement complete ===\n");
            vTaskSuspend(NULL);
        }
    }
}

void TaskB(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void test_create_tasks(void)
{
    xTaskCreate(TaskA, "TaskA", 256, NULL, 2, NULL);
    xTaskCreate(TaskB, "TaskB", 256, NULL, 1, NULL);
}

void test_print_statistics(void)
{
    uart_print("\n=== Performance Statistics ===\n");
    uart_print("Total switches: ");
    uart_printf("%lu\n", switch_count);
    uart_print("Average cycles: ");
    uart_printf("%lu\n", (uint32_t)(total_cycles / switch_count));
    uart_print("Min cycles: ");
    uart_printf("%lu\n", min_cycles);
    uart_print("Max cycles: ");
    uart_printf("%lu\n", max_cycles);
    uart_print("At 50MHz: ~");
    uart_printf("%lu", (uint32_t)((total_cycles / switch_count) / 50));
    uart_print(" us per switch\n");
    uart_print("==============================\n");
}
