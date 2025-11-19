# Baseline and Static MPU Configuration

This directory contains implementations that use the **original FreeRTOS kernel** (unmodified).

## Configurations

### Baseline (No MPU)
- MPU is **disabled**
- Used to measure baseline performance

Build: `make CONFIG=baseline`

### Static MPU
- MPU is configured **once at boot time**
- Memory regions never change at runtime
- 3-4 fixed regions: Flash (code), SRAM (data), Peripherals

Build: `make CONFIG=static`

## Files

- `src/main.c` - Main entry point (uses CONFIG macro)
- `src/mpu_static.c` - Static MPU configuration
- `freertos/` - Original FreeRTOS kernel (unmodified)

