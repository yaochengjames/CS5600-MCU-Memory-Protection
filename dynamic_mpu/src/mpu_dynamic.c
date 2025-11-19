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
