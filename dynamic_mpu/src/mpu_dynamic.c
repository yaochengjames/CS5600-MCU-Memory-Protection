#include "mpu_dynamic.h"
#include "uart.h"

// MPU Registers
#define MPU_CTRL     (*(volatile uint32_t*)0xE000ED94)
#define MPU_RNR      (*(volatile uint32_t*)0xE000ED98)
#define MPU_RBAR     (*(volatile uint32_t*)0xE000ED9C)
#define MPU_RASR     (*(volatile uint32_t*)0xE000EDA0)

