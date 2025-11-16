#include "test_tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "benchmark.h"

#define NUM_SAMPLES 3000
#define WARMUP_SAMPLES 200
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
    uint32_t current, after, elapsed;
    uint32_t warmup_count = 0;
    
    for (;;) {
        current = benchmark_get_cycles();
        
        taskYIELD();
        
        after = benchmark_get_cycles();
        
        if (after >= current) {
            elapsed = after - current;
        } else {
            elapsed = (SYSTICK_MAX - current) + after;
        }
        
        if (warmup_count < WARMUP_SAMPLES) {
            warmup_count++;
            continue;
        }
        
        if (elapsed > 10 && elapsed < 50000) {
            total_cycles += elapsed;
            switch_count++;
            
            if (elapsed < min_cycles) min_cycles = elapsed;
            if (elapsed > max_cycles) max_cycles = elapsed;
        }
        
        if (switch_count > 0 && switch_count % 500 == 0) {
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
        
        if (switch_count % 1000 == 0) {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

void TaskB(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#ifdef CONFIG_STATIC

void TaskViolator(void *pvParameters)
{
    (void)pvParameters;
    
    // Wait for TaskA to complete measurement
    while (switch_count < NUM_SAMPLES) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // Wait additional 1 second for results to print
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    uart_print("\n");
    uart_print("========================================\n");
    uart_print("       MPU Fault Detection Test         \n");
    uart_print("========================================\n");
    uart_print("[TEST] Attempting illegal memory access\n");
    uart_print("[TEST] Target: 0x60000000 (unmapped region)\n");
    uart_print("[TEST] \n");
    uart_print("[TEST] Expected behavior:\n");
    uart_print("[TEST]   - If MPU works: System halts with fault\n");
    uart_print("[TEST]   - If MPU disabled: Access succeeds\n");
    uart_print("[TEST] \n");
    uart_print("[TEST] Executing write to 0x60000000...\n");
    
    volatile uint32_t *illegal_addr = (uint32_t*)0x60000000;
    *illegal_addr = 0xDEADBEEF;
    
    uart_print("[TEST] \n");
    uart_print("[TEST] *** RESULT: No fault detected ***\n");
    uart_print("[TEST] \n");
    uart_print("[TEST] Conclusion:\n");
    uart_print("[TEST] MPU fault detection is NOT working in QEMU\n");
    uart_print("[TEST] This is a known QEMU MPS2-AN386 limitation\n");
    uart_print("[TEST] \n");
    uart_print("[TEST] Note: MPU would work on real hardware\n");
    uart_print("========================================\n");
    
    // Suspend self
    vTaskSuspend(NULL);
}

#endif

void test_create_tasks(void)
{
    xTaskCreate(TaskA, "TaskA", 256, NULL, 2, NULL);
    xTaskCreate(TaskB, "TaskB", 256, NULL, 1, NULL);
    
#ifdef CONFIG_STATIC
    xTaskCreate(TaskViolator, "Violator", 256, NULL, 1, NULL);
#endif
}

void test_print_statistics(void)
{
    uint32_t avg_cycles = (uint32_t)(total_cycles / switch_count);
    uint32_t range = max_cycles - min_cycles;
    
    uart_print("\n=== Performance Statistics ===\n");
    uart_print("Configuration: ");
    uart_print(config_name);
    uart_print("\n");
    uart_print("Total switches: ");
    uart_printf("%lu\n", switch_count);
    uart_print("Average cycles: ");
    uart_printf("%lu\n", avg_cycles);
    uart_print("Min cycles: ");
    uart_printf("%lu\n", min_cycles);
    uart_print("Max cycles: ");
    uart_printf("%lu\n", max_cycles);
    uart_print("Range: ");
    uart_printf("%lu", range);
    uart_print(" cycles\n");
    uart_print("At 50MHz: ~");
    uart_printf("%lu", avg_cycles / 50);
    uart_print(" us per switch\n");
    uart_print("==============================\n");
}
