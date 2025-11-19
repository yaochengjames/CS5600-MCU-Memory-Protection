#ifndef MPU_DYNAMIC_H
#define MPU_DYNAMIC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    uint32_t rbar[8];
    uint32_t rasr[8];
    uint8_t num_regions;
    const char* task_name;
} mpu_task_config_t;

void mpu_dynamic_init(void);
void mpu_dynamic_switch_task(void *task_handle);
uint32_t mpu_dynamic_get_switches(void);
void mpu_dynamic_print_config(void);

#endif
