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
#define MPU_CTRL_PRIVDEFENA   (1 << 2)  // Enable default memory map

// Region attributes (from Composite paper §II-B)
#define MPU_REGION_ENABLE     (1 << 0)
#define MPU_REGION_SIZE(n)    ((n) << 1)  // Size = 2^(n+1) bytes

// Access permissions (AP bits)
#define MPU_AP_PRIV_RW_USER_NO   (1 << 24)  // Privileged R/W, User none
#define MPU_AP_PRIV_RW_USER_RW   (3 << 24)  // Full access
#define MPU_AP_PRIV_RO_USER_NO   (5 << 24)  // Privileged RO
#define MPU_AP_PRIV_RO_USER_RO   (6 << 24)  // Read-only

// Execute Never bit
#define MPU_XN                (1 << 28)

// Region sizes (Size = 2^(n+1))
// For n=20: size = 2^21 = 2MB
// For n=18: size = 2^19 = 512KB
#define MPU_SIZE_2MB      20
#define MPU_SIZE_512KB    18
#define MPU_SIZE_256KB    17

void mpu_static_init(void)
{
    uart_print("[MPU] Initializing Static MPU configuration...\n");
    
    // Disable MPU first
    MPU_CTRL = 0;
    
    // Region 0: Flash (0x00000000 - 0x00200000, 2MB)
    // Read-only, Executable
    MPU_RNR = 0;
    MPU_RBAR = 0x00000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(MPU_SIZE_2MB) |
               MPU_AP_PRIV_RO_USER_RO;
    
    // Region 1: SRAM (0x20000000 - 0x20080000, 512KB)
    // Read-write, Non-executable
    MPU_RNR = 1;
    MPU_RBAR = 0x20000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(MPU_SIZE_512KB) |
               MPU_AP_PRIV_RW_USER_RW |
               MPU_XN;
    
    // Region 2: Peripherals (0x40000000 - 0x50000000, 256MB)
    // Read-write, Non-executable
    MPU_RNR = 2;
    MPU_RBAR = 0x40000000;
    MPU_RASR = MPU_REGION_ENABLE | 
               MPU_REGION_SIZE(27) |  // 2^28 = 256MB
               MPU_AP_PRIV_RW_USER_RW |
               MPU_XN;
    
    uart_print("[MPU] Configured 3 regions:\n");
    uart_print("  Region 0: Flash   [0x00000000-0x00200000] R+X\n");
    uart_print("  Region 1: SRAM    [0x20000000-0x20080000] R+W\n");
    uart_print("  Region 2: Periph  [0x40000000-0x50000000] R+W\n");
}

void mpu_static_enable(void)
{
    // Enable MPU with default memory map for privileged access
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
    uint32_t type = MPU_TYPE;
    uint32_t num_regions = (type >> 8) & 0xFF;
    
    uart_print("\n[MPU] Hardware Information:\n");
    uart_printf("  Available regions: %lu\n", num_regions);
    uart_printf("  MPU TYPE register: 0x%lx\n", type);
    
    uart_print("\n[MPU] Static Configuration:\n");
    uart_print("  Total accessible memory: ~2.5 MB\n");
    uart_print("  Memory reduction: 0% (same for all tasks)\n");
    uart_print("  Isolation level: Coarse-grained\n");
}
