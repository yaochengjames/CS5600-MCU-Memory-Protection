# Memory Configuration Analysis

## Baseline Configuration
- Total addressable: 2 MB (Flash) + 512 KB (SRAM) + 256 MB (Peripherals)
- Accessible per task: **All memory (100%)**
- Isolation: None

## Static MPU Configuration
- Region 0: Flash 0x00000000-0x00200000 (2 MB) - All tasks
- Region 1: SRAM 0x20000000-0x20080000 (512 KB) - All tasks
- Region 2: Peripherals 0x40000000-0x50000000 (256 MB) - All tasks
- Accessible per task: **All memory (100%)**
- Isolation: Coarse-grained (prevents accidental out-of-range access)
- Memory reduction: **0%** (all tasks share same view)

## Dynamic MPU Configuration (To be measured)
- TaskA regions: TBD
- TaskB regions: TBD
- Accessible per task: TBD
- Memory reduction: TBD
