#include "mpu_static.h"
#include "uart.h"

// MPU Registers (from Composite paper §V-A, page 6)
#define MPU_TYPE     (*(volatile uint32_t*)0xE000ED90)
#define MPU_CTRL     (*(volatile uint32_t*)0xE000ED94)
#define MPU_RNR      (*(volatile uint32_t*)0xE000ED98)
#define MPU_RBAR     (*(volatile uint32_t*)0xE000ED9C)
#define MPU_RASR     (*(volatile uint32_t*)0xE000EDA0)

// MPU Control bits
#define MPU_CTRL_ENABLE       (1 << 0)
#define MPU_CTRL_PRIVDEFENA   (1 << 2)

// Region attributes (from Composite paper §II-B)
#define MPU_REGION_ENABLE     (1 << 0)
#define MPU_REGION_SIZE(n)    ((n) << 1)

// Access permissions (AP bits)
#define MPU_AP_PRIV_RW_USER_NO   (1 << 24)
#define MPU_AP_PRIV_RW_USER_RW   (3 << 24)
#define MPU_AP_PRIV_RO_USER_NO   (5 << 24)
#define MPU_AP_PRIV_RO_USER_RO   (6 << 24)

// Execute Never bit
#define MPU_XN                (1 << 28)

// Region sizes
#define MPU_SIZE_2MB      20
#define MPU_SIZE_512KB    18
#define MPU_SIZE_256KB    17

// MPU fault counter
static volatile uint32_t mpu_fault_count = 0;

void mpu_static_init(void)
{
    uart_print("[MPU] Initializing Static MPU configuration...\n");
    
    MPU_CTRL = 0;
    
    MPU_RNR = 0;
    MPU_RBAR = 0x00000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(MPU_SIZE_2MB) |
               MPU_AP_PRIV_RO_USER_RO;
    
    MPU_RNR = 1;
    MPU_RBAR = 0x20000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(MPU_SIZE_512KB) |
               MPU_AP_PRIV_RW_USER_RW |
               MPU_XN;
    
    MPU_RNR = 2;
    MPU_RBAR = 0x40000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(27) |
               MPU_AP_PRIV_RW_USER_RW |
               MPU_XN;
    
    uart_print("[MPU] Configured 3 regions:\n");
    uart_print("  Region 0: Flash   [0x00000000-0x00200000] R+X\n");
    uart_print("  Region 1: SRAM    [0x20000000-0x20080000] R+W\n");
    uart_print("  Region 2: Periph  [0x40000000-0x50000000] R+W\n");
}

void mpu_static_enable(void)
{
    MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;
    uart_print("[MPU] MPU enabled\n");
}

void mpu_static_disable(void)
{
    MPU_CTRL = 0;
    uart_print("[MPU] MPU disabled\n");
}

void mpu_static_print_config(void)
{   
    uart_print("\n[MPU] Static Configuration Summary:\n");
    uart_print("  Regions configured: 3\n");
    uart_print("  Total accessible memory: ~2.5 MB\n");
    uart_print("  Memory reduction: 0%\n");
    uart_print("  Note: Same config for all tasks\n");
}

void MemManage_Handler(void)
{
    mpu_fault_count++;
    
    uart_print("\n");
    uart_print("========================================\n");
    uart_print("!!! MPU FAULT DETECTED !!!\n");
    uart_print("========================================\n");
    uart_print("Fault count: ");
    uart_printf("%lu\n", mpu_fault_count);
    uart_print("\n");
    uart_print("[TEST] *** SUCCESS: MPU is working! ***\n");
    uart_print("[TEST] Illegal memory access was blocked\n");
    uart_print("[TEST] System halting as expected\n");
    uart_print("\n");
    uart_print("Press Ctrl+C to exit QEMU\n");
    uart_print("========================================\n");
    
    for (;;);
}

uint32_t mpu_get_fault_count(void)
{
    return mpu_fault_count;
}
