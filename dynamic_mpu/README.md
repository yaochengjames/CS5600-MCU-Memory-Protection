# Dynamic MPU Configuration

This directory contains implementation that uses **modified FreeRTOS kernel**.

## Key Difference

- MPU is **reconfigured on every context switch**
- Each task has its own memory view
- Requires modification of `port.c` in FreeRTOS

Build: `make`

## Modified Files

- `freertos/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c`
  - Modified `xPortPendSVHandler` to switch MPU on context switch

## Implementation Status

- [ ] Prepare per-task MPU configurations
- [ ] Modify `xPortPendSVHandler` in port.c
- [ ] Test MPU switching overhead

