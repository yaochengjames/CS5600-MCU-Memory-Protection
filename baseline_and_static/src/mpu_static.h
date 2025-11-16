#ifndef MPU_STATIC_H
#define MPU_STATIC_H

#include <stdint.h>

void mpu_static_init(void);
void mpu_static_enable(void);
void mpu_static_disable(void);
void mpu_static_print_config(void);
uint32_t mpu_get_fault_count(void);

#endif
