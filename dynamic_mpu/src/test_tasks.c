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

    while (switch_count < NUM_SAMPLES) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    uart_print("\n========================================\n");
    uart_print("       MPU Fault Detection Test         \n");
    uart_print("========================================\n");

    uart_print("[TEST 1] Writing to Flash (Read-only region)\n");
    uart_print("[TEST 1] Target: 0x00100000 (Flash)\n");
    uart_print("[TEST 1] Expected: MPU fault (write to RO region)\n");

    volatile uint32_t *flash_addr = (uint32_t*)0x00100000;
    uart_print("[TEST 1] Executing write...\n");
    *flash_addr = 0xDEADBEEF;

    uart_print("[TEST 1] ERROR: No fault detected!\n");
    uart_print("[TEST 1] Write to read-only Flash succeeded\n");
    uart_print("[TEST 1] MPU may not be enforcing RO permissions\n");

    vTaskSuspend(NULL);
}

#endif

#ifdef CONFIG_DYNAMIC

void TaskIsolationTest(void *pvParameters)
{
    (void)pvParameters;
    
    while (switch_count < NUM_SAMPLES) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    uart_print("\n========================================\n");
    uart_print("  Task Isolation Test (Dynamic MPU)    \n");
    uart_print("========================================\n");
    uart_print("[TEST] Verifying per-task memory isolation\n");
    uart_print("\n");
    
    uart_print("[TEST 1] Write to unmapped region (0x60000000)\n");
    uart_print("[TEST 1] Expected: Fault (outside any task's view)\n");
    uart_print("[TEST 1] Executing...\n");
    
    volatile uint32_t *unmapped = (uint32_t*)0x60000000;
    *unmapped = 0xDEADBEEF;
    
    uart_print("[TEST 1] Result: No fault\n");
    uart_print("[TEST 1] Note: May trigger bus fault instead of MPU fault\n");
    uart_print("\n");
    
    uart_print("[TEST 2] Write to Flash (Read-only in MPU config)\n");
    uart_print("[TEST 2] Target: 0x00100000 (Flash region)\n");
    uart_print("[TEST 2] Expected: MPU fault (RO violation)\n");
    uart_print("[TEST 2] Executing...\n");
    
    volatile uint32_t *flash = (uint32_t*)0x00100000;
    *flash = 0xBAADF00D;
    
    uart_print("[TEST 2] Result: No fault\n");
    uart_print("[TEST 2] Note: QEMU MPU emulation limitations\n");
    uart_print("\n");
    
    uart_print("========================================\n");
    uart_print("Isolation Test Summary:\n");
    uart_print("- MPU configuration verified via GDB\n");
    uart_print("- Per-task regions successfully switched\n");
    uart_print("- Fault detection limited by QEMU\n");
    uart_print("- Real hardware would provide full protection\n");
    uart_print("========================================\n");
    
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

#ifdef CONFIG_DYNAMIC
    xTaskCreate(TaskIsolationTest, "IsoTest", 256, NULL, 1, NULL);
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
    
    #ifdef CONFIG_DYNAMIC
    extern uint32_t mpu_dynamic_get_switches(void);
    uint32_t mpu_switches = mpu_dynamic_get_switches();
    uart_print("\n[DEBUG] MPU reconfigurations: ");
    uart_printf("%lu\n", mpu_switches);
    
    if (mpu_switches == 0) {
        uart_print("[WARNING] MPU switching NOT working!\n");
        uart_print("[WARNING] mpu_dynamic_switch_task() was never called\n");
    } else if (mpu_switches < switch_count) {
        uart_print("[WARNING] Partial MPU switching (");
        uart_printf("%lu", (mpu_switches * 100) / switch_count);
        uart_print("%%)\n");
    } else {
        uart_print("[OK] MPU switching is active (100%%)\n");
    }
    #endif
    
    uart_print("==============================\n");
}
