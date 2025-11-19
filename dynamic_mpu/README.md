# Dynamic MPU Configuration

This directory implements **per-task memory isolation** using dynamic MPU reconfiguration.

## Key Difference from Static MPU

- **Static MPU**: All tasks share the same memory view (2.5 MB)
- **Dynamic MPU**: Each task has isolated memory view (~20-30 KB per task)

## Implementation

- MPU is **reconfigured on every context switch**
- Each task can only access its own code, data, stack
- Requires modification of `port.c` in FreeRTOS kernel

## Modified Files

- `freertos/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c`
  - Added MPU switching in `xPortPendSVHandler`
- `src/mpu_dynamic.c` - Per-task MPU configuration
- `src/main.c` - Uses CONFIG_DYNAMIC

## Build
```bash
make clean
make
make run
```
