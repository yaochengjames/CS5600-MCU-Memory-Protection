# Memory Accessibility Analysis

## Methodology
Memory accessibility is determined by MPU region configuration.
Each value represents the total address space accessible to a task.

## Results

### Baseline Configuration (No MPU)
- **All memory accessible**: 2.5 MB
  - Flash: 2 MB
  - SRAM: 512 KB  
  - Peripherals: 256 MB (MMIO)
- **Per-task accessibility**: 100% (no restrictions)

### Static MPU Configuration
- **Regions configured**: 3 (shared by all tasks)
  - Region 0: Flash 0x00000000-0x00200000 (2 MB)
  - Region 1: SRAM 0x20000000-0x20080000 (512 KB)
  - Region 2: Peripherals 0x40000000-0x50000000 (256 MB)
- **Per-task accessibility**: 2.5 MB (100%, same as baseline)
- **Memory reduction**: 0%

### Dynamic MPU Configuration

**TaskA:**
- Region 0: Flash 0x00000000-0x00008000 (32 KB)
- Region 1: SRAM 0x20000000-0x20080000 (512 KB)
- Region 2: UART 0x40004000-0x40005000 (4 KB)
- **Total accessible**: 548 KB
- **Reduction**: 78.4% (vs 2.5 MB baseline)

**TaskB:**
- Region 0: Flash 0x00000000-0x00004000 (16 KB)
- Region 1: SRAM 0x20000000-0x20080000 (512 KB)
- Region 2: UART 0x40004000-0x40005000 (4 KB)
- **Total accessible**: 532 KB
- **Reduction**: 78.7% (vs 2.5 MB baseline)

## Summary Table

| Configuration | Memory/Task | Reduction | Isolation Level |
|--------------|-------------|-----------|----------------|
| Baseline | 2.5 MB (100%) | 0% | None |
| Static | 2.5 MB (100%) | 0% | Coarse-grained |
| **Dynamic** | **~540 KB** | **~78%** | **Per-task** |


