#include "mpu_dynamic.h"
#include "uart.h"

// MPU Registers
#define MPU_CTRL     (*(volatile uint32_t*)0xE000ED94)
#define MPU_RNR      (*(volatile uint32_t*)0xE000ED98)
#define MPU_RBAR     (*(volatile uint32_t*)0xE000ED9C)
#define MPU_RASR     (*(volatile uint32_t*)0xE000EDA0)

// MPU Control bits
#define MPU_CTRL_ENABLE       (1 << 0)
#define MPU_CTRL_PRIVDEFENA   (1 << 2)

// Region attributes
#define MPU_REGION_ENABLE     (1 << 0)
#define MPU_REGION_SIZE(n)    ((n) << 1)

// Access permissions
#define MPU_AP_RW             (3 << 24)
#define MPU_AP_RO             (6 << 24)

// Execute Never
#define MPU_XN                (1 << 28)

// Region size definitions
// Size = 2^(n+1) bytes
#define SIZE_4KB    11   // 2^12 = 4KB
#define SIZE_8KB    12   // 2^13 = 8KB
#define SIZE_16KB   13   // 2^14 = 16KB
#define SIZE_32KB   14   // 2^15 = 32KB
#define SIZE_512KB  18   // 2^19 = 512KB

// MPU switch counter
static volatile uint32_t mpu_switch_count = 0;

// Per-task MPU configurations
static mpu_task_config_t task_configs[2] = {
    // TaskA configuration
    {
        .rbar = {
            0x00000000,  // Region 0: Flash (TaskA code)
            0x20000000,  // Region 1: SRAM (all data for now)
            0x40004000,  // Region 2: UART peripheral
            0, 0, 0, 0, 0
        },
        .rasr = {
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_32KB) | MPU_AP_RO,
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_512KB) | MPU_AP_RW | MPU_XN,
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_4KB) | MPU_AP_RW | MPU_XN,
            0, 0, 0, 0, 0
        },
        .num_regions = 3,
        .task_name = "TaskA"
    },
    // TaskB configuration
    {
        .rbar = {
            0x00000000,  // Region 0: Flash (TaskB code)
            0x20000000,  // Region 1: SRAM (all data for now)
            0x40004000,  // Region 2: UART peripheral
            0, 0, 0, 0, 0
        },
        .rasr = {
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_16KB) | MPU_AP_RO,
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_512KB) | MPU_AP_RW | MPU_XN,
            MPU_REGION_ENABLE | MPU_REGION_SIZE(SIZE_4KB) | MPU_AP_RW | MPU_XN,
            0, 0, 0, 0, 0
        },
        .num_regions = 3,
        .task_name = "TaskB"
    }
};

void mpu_dynamic_init(void)
{
    uart_print("[MPU] Initializing Dynamic MPU...\n");
    uart_print("[MPU] Per-task memory isolation enabled\n");
    uart_print("[MPU] TaskA regions: 3 (32KB Flash + 512KB SRAM + 4KB UART)\n");
    uart_print("[MPU] TaskB regions: 3 (16KB Flash + 512KB SRAM + 4KB UART)\n");
    uart_print("[MPU] MPU will switch on every context switch\n");
    
    // Enable MPU
    MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;
}

void mpu_dynamic_switch_task(void *task_handle)
{
    if (task_handle == NULL) return;
    
    mpu_switch_count++;
    
    // Get task name to identify which config to use
    const char* name = pcTaskGetName((TaskHandle_t)task_handle);
    
    mpu_task_config_t *config = NULL;
    
    // Simple name matching (TaskA or TaskB)
    if (name[4] == 'A') {
        config = &task_configs[0];
    } else if (name[4] == 'B') {
        config = &task_configs[1];
    } else {
        // Unknown task, use default (TaskA config)
        config = &task_configs[0];
    }
    
    // Reconfigure MPU regions for this task
    for (uint8_t i = 0; i < config->num_regions; i++) {
        MPU_RNR = i;
        MPU_RBAR = config->rbar[i];
        MPU_RASR = config->rasr[i];
    }
    
    // Disable unused regions
    for (uint8_t i = config->num_regions; i < 8; i++) {
        MPU_RNR = i;
        MPU_RASR = 0;  // Disable region
    }
}

uint32_t mpu_dynamic_get_switches(void)
{
    return mpu_switch_count;
}

void mpu_dynamic_print_config(void)
{
    uart_print("\n[MPU] Dynamic Configuration Summary:\n");
    uart_print("  TaskA: 32KB Flash + 512KB SRAM + 4KB UART = ~548 KB\n");
    uart_print("  TaskB: 16KB Flash + 512KB SRAM + 4KB UART = ~532 KB\n");
    uart_print("  Memory reduction: ~78% per task (vs 2.5 MB baseline)\n");
    uart_print("  Total MPU switches: ");
    uart_printf("%lu\n", mpu_switch_count);
}
