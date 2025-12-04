# MPU Configuration Comparison Results

## Experimental Results

### Measurement Methodology

**Approach (taskYIELD):**
- Measures pure context switch overhead
- Timer: SysTick (24-bit down counter)
- Samples: 3,000 per configuration (after 200 warmup)
- Outlier filtering: 10-50,000 cycles range
- Platform: QEMU MPS2-AN386 (Cortex-M4 @ 50MHz)

---

### Context Switch Latency

| Configuration | Avg Cycles | Min | Max | Overhead |
|---------------|------------|-----|------|----------|
| Baseline | 83 | 24 | 1,750 | - |
| Static MPU | 91 | 24 | 150 | +9.6% |
| Dynamic MPU | 685 | 24 | 1,400 | +725% |

---

### Memory Accessibility per Task

| Configuration | Accessible Memory | Reduction | Isolation |
|---------------|-------------------|-----------|-----------|
| Baseline | 2.5 MB (all memory) | 0% | None |
| Static MPU | 2.5 MB (shared regions) | 0% | Coarse-grained |
| Dynamic MPU | ~540 KB | 78% | Per-task |

**Dynamic MPU Details:**
- TaskA: 32KB Flash + 512KB SRAM + 4KB UART = 548 KB
- TaskB: 16KB Flash + 512KB SRAM + 4KB UART = 532 KB

---

### MPU Fault Detection

| Configuration | Test | Result | Verified |
|---------------|------|--------|----------|
| Baseline | N/A | No protection | - |
| Static MPU | Write to Flash (RO) | System halts ✓ | GDB + Runtime |
| Dynamic MPU | Write to unmapped region | System halts ✓ | GDB + Runtime |

---

### GDB Verification Summary

**Static MPU:**
- MPU_CTRL = 0x5 (enabled)
- CFSR = 0x82 (DACCVIOL)

**Dynamic MPU:**
- MPU_CTRL = 0x5 (enabled)
- RASR[0] changes: 0x600001D ↔ 0x6000019 (TaskA ↔ TaskB)
- MPU reconfigurations: 5,605 switches recorded

---

### Key Findings

1. **Static MPU:** Minimal overhead (+9.6%), basic protection, no memory reduction

2. **Dynamic MPU:** Significant overhead (+725%), strong per-task isolation, 78% memory reduction

3. **Both configurations** successfully detect and halt on illegal memory accesses

4. **At 50MHz:** Static = ~1.8 μsec, Dynamic = ~14 μsec per context switch

---

### Comparison with Literature

| Metric | Our Result | MINION (NDSS 2018) |
|--------|------------|-------------------|
| Memory reduction | 78% | 76-93% |
| Switch overhead | ~14 μsec | ~15 μsec |

Results consistent with prior research.

---

### Limitations

- QEMU emulation: Fault types may vary from real hardware
- Only 2 tasks evaluated
- Tasks share full 512KB SRAM region
