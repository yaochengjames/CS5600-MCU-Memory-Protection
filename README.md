# Efficient Memory Isolation Strategies for Real-Time Embedded Systems



## Project Overview

This project compares three Memory Protection Unit (MPU) strategies on FreeRTOS running on ARM Cortex-M4, addressing the research question:

> **How can a real-time operating system use limited MPU regions to isolate tasks effectively without breaking timing constraints?**

**Key findings:**
- **Baseline**: 83 cycles, 2.5 MB accessible, 0% isolation
- **Static MPU**: 91 cycles (+9.6%), 2.5 MB accessible, coarse-grained protection
- **Dynamic MPU**: 669 cycles (+706%), ~540 KB accessible, **78% memory reduction**

---

## Project Structure
```
CS5600-MCU-Memory-Protection/
├── baseline_and_static/        # Baseline and Static MPU (shared directory)
│   ├── src/                    # Source code
│   │   ├── main.c
│   │   ├── mpu_static.c/h      # Static MPU implementation
│   │   ├── uart.c/h            # UART driver
│   │   ├── benchmark.c/h       # SysTick-based measurement
│   │   ├── test_tasks.c/h      # Performance tests
│   │   └── startup_cm4.S       # ARM startup
│   ├── freertos/               # Original FreeRTOS (unmodified)
│   ├── Makefile                # Supports CONFIG=baseline/static
│   └── link.ld
│
├── dynamic_mpu/                # Dynamic MPU (modified FreeRTOS)
│   ├── src/
│   │   ├── main.c
│   │   ├── mpu_dynamic.c/h     # Dynamic MPU implementation
│   │   └── ... (other files)
│   ├── freertos/
│   │   └── .../port.c          # Modified for MPU switching
│   ├── Makefile                # Fixed to DYNAMIC config
│   └── link.ld
│
└── results/                    # Experimental results
    ├── baseline_yield*.log
    ├── static_yield*.log
    ├── dynamic_final*.log
    ├── summary.md
    ├── memory_analysis.md
    ├── mpu_verification_gdb.md
    └── fault_detection_summary.md
```

---

## Quick Start

### Prerequisites
- ARM GNU Toolchain (arm-none-eabi-gcc)
- QEMU (qemu-system-arm)
- Make

### Build and Run

#### Baseline Configuration
```bash
cd baseline_and_static
make CONFIG=baseline
make CONFIG=baseline run
```

#### Static MPU Configuration
```bash
cd baseline_and_static
make CONFIG=static
make CONFIG=static run
```

#### Dynamic MPU Configuration
```bash
cd dynamic_mpu
make
make run
```

---

## Experimental Results

### Context Switch Latency

| Configuration | Average Cycles | Min | Max | Overhead |
|--------------|---------------|-----|-----|----------|
| Baseline | 83 | 100 | 1,750 | - |
| Static MPU | 91 | 24 | 150 | **+9.6%** |
| Dynamic MPU | 669 | 24 | 6,450 | **+706%** |

**Measurement:** 3,000 samples using `taskYIELD()` and SysTick timer

---

### Memory Accessibility per Task

| Configuration | Accessible Memory | Reduction | Isolation |
|--------------|------------------|-----------|-----------|
| Baseline | 2.5 MB (all memory) | 0% | None |
| Static MPU | 2.5 MB (shared regions) | 0% | Coarse-grained |
| **Dynamic MPU** | **~540 KB** | **78%** | **Per-task** |

**Dynamic MPU Details:**
- TaskA: 32KB Flash + 512KB SRAM + 4KB UART = 548 KB
- TaskB: 16KB Flash + 512KB SRAM + 4KB UART = 532 KB

---

### MPU Fault Detection

| Configuration | Test | Result | Verified |
|--------------|------|--------|----------|
| Baseline | N/A | No protection | - |
| Static | Write to Flash (RO) | System halts | GDB + Runtime |
| Dynamic | Write to unmapped region | System halts | GDB + Runtime |

**Verification:** GDB shows MPU configurations change per task (5,605 switches recorded)

---

## Key Insights

### Performance vs Security Trade-off
```
Dynamic MPU provides:
  ✅ Fault detection (strong protection)  
  ✅ 78% memory reduction (strong isolation)
  ❌ 706% performance overhead (7x slower)

Static MPU provides:
  ✅ Fault detection (basic protection)
  ✅ 9.6% overhead (minimal impact)
  ❌ Memeory isolation coarse-grained
```

### Hardware Constraints

- ARM Cortex-M4: **8 MPU regions maximum**
- Our usage: 3 regions per task (Dynamic)
- Remaining capacity: ~2 more tasks possible

---

## Technical Details

### Platform
- **Emulator:** QEMU MPS2-AN386
- **CPU:** ARM Cortex-M4 @ 50MHz
- **RTOS:** FreeRTOS v10.6
- **MPU:** 8 regions (hardware limit)

### Measurement Tools
- **Timer:** SysTick (24-bit counter)
- **Debugger:** arm-none-eabi-gdb
- **Method:** taskYIELD() for pure context switch measurement

### MPU Registers
- MPU_CTRL, MPU_RNR, MPU_RBAR, MPU_RASR
- Verified via GDB remote debugging

---

## Limitations

### QEMU Emulation
- Fault types may vary from real hardware (UsageFault vs MemManage)
- DWT/CoreSight not supported

---

## References

- [1] P. Mackensen, C. Niesler, R. Blanco, L. Davi, and V. Moonsamy, “KINTSUGI: Secure Hotpatching for Code-
Shadowing Real-Time Embedded Systems,” Proc. 34th USENIX Security Symp., USENIX, 2025. Available:
https://www.usenix.org/conference/usenixsecurity25/presentation/mackensen
- [2] C. H. Kim, T. Kim, H. Choi, Z. Gu, B. Lee, X. Zhang, and D. Xu, “Securing Real-Time Microcontroller Systems
through Customized Memory View Switching,” Proc. NDSS 2018, Internet Society, 2018. Available: https://www.ndss-symposium.org/wp-content/uploads/2018/02/ndss2018_04B-2_Kim_paper.pdf
- [3] R. Pan, G. Peach, Y. Ren, and G. Parmer, “Predictable Virtualization on Memory Protection Unit-Based
Microcontrollers,” Proc. IEEE RTAS 2018. Available: https://www2.seas.gwu.edu/~gparmer/publications/rtas18mpu.pdf
- [4] W. Zhou, Z. Jiang, and L. Guan, “Understanding MPU Usage in Microcontroller-based Systems in the Wild,” NDSS
BAR Workshop 2023, Internet Society, 2023. Available: https://www.ndss-symposium.org/ndss-paper/auto-draft-395/
- [5] ARM Ltd., ARM Cortex-M Architecture Reference Manual – Memory Protection Unit (MPU) Chapter, ARM
Technical Documentation. Available: https://developer.arm.com/documentation/107565/0101/Memory-protection/Memory-Protection-Unit
- [6] FreeRTOS Documentation, “Memory Protection Unit (MPU) Support,” FreeRTOS.org. Available:
https://www.freertos.org/Security/04-FreeRTOS-MPU-memory-protection-unit

---
