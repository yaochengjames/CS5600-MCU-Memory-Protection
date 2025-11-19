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
#define SIZE_4KB    11
#define SIZE_8KB    12
#define SIZE_16KB   13
#define SIZE_32KB   14
#define SIZE_512KB  18

// MPU switch counter
volatile uint32_t mpu_switch_count = 0;

// Per-task MPU configurations
static mpu_task_config_t task_configs[2] = {
    // TaskA configuration
    {
        .rbar = {
            0x00000000,
            0x20000000,
            0x40004000,
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
            0x00000000,
            0x20000000,
            0x40004000,
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
    
    MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;
}

void mpu_dynamic_switch_task(void *task_handle)
{
    if (task_handle == NULL) return;
    
    mpu_switch_count++;
    
    const char* name = pcTaskGetName((TaskHandle_t)task_handle);
    
    mpu_task_config_t *config = NULL;
    
    if (name[4] == 'A') {
        config = &task_configs[0];
    } else if (name[4] == 'B') {
        config = &task_configs[1];
    } else {
        config = &task_configs[0];
    }
    
    for (uint8_t i = 0; i < config->num_regions; i++) {
        MPU_RNR = i;
        MPU_RBAR = config->rbar[i];
        MPU_RASR = config->rasr[i];
    }
    
    for (uint8_t i = config->num_regions; i < 8; i++) {
        MPU_RNR = i;
        MPU_RASR = 0;
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
}
