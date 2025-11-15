#include "test_tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"

#define NUM_SAMPLES 1000

static volatile uint32_t switch_count = 0;
static volatile uint64_t total_cycles = 0;
static volatile uint32_t min_cycles = 0xFFFFFFFF;
static volatile uint32_t max_cycles = 0;

void TaskA(void *pvParameters)
{
    (void)pvParameters;
    uint32_t start, elapsed;
    
    for (;;) {
        start = benchmark_get_cycles();
        
        // Force context switch
        vTaskDelay(pdMS_TO_TICKS(10));
        
        elapsed = benchmark_get_cycles() - start;
        
        // Update statistics
        total_cycles += elapsed;
        switch_count++;
        
        if (elapsed < min_cycles) min_cycles = elapsed;
        if (elapsed > max_cycles) max_cycles = elapsed;
        
        // Print progress every 100 switches
        if (switch_count % 100 == 0) {
            uart_printf("[Baseline] Switches: %lu, Avg: %lu cycles\n", 
                       switch_count, 
                       (uint32_t)(total_cycles / switch_count));
        }
        
        // Stop after NUM_SAMPLES
        if (switch_count >= NUM_SAMPLES) {
            test_print_statistics();
            uart_print("\n=== Baseline measurement complete ===\n");
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
    uart_printf("Total switches: %lu\n", switch_count);
    uart_printf("Average cycles: %lu\n", (uint32_t)(total_cycles / switch_count));
    uart_printf("Min cycles: %lu\n", min_cycles);
    uart_printf("Max cycles: %lu\n", max_cycles);
    uart_print("==============================\n");
}
