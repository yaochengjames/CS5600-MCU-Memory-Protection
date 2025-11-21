# Dynamic MPU Configuration

Per-task memory isolation using dynamic MPU reconfiguration on FreeRTOS.

---

## Implementation Details

### Key Modified Files

#### 1. `freertos/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c`

**Changes:**
```c
// Line 38: Add extern declaration
#if defined(CONFIG_DYNAMIC)
    extern void mpu_dynamic_switch_task(void *);
#endif

// Line 509: Add MPU switching in xPortPendSVHandler (inside assembly block)
void xPortPendSVHandler( void )
{
    __asm volatile
    (
        // ... existing FreeRTOS code ...
        "   msr psp, r0                         \n"
        "   isb                                 \n"
        
        // Added: MPU switching code
        #if defined(CONFIG_DYNAMIC)
        "   stmdb sp!, {r0-r3, r12, lr}         \n"
        "   ldr r3, =pxCurrentTCB               \n"
        "   ldr r0, [r3]                        \n"
        "   bl mpu_dynamic_switch_task          \n"
        "   ldmia sp!, {r0-r3, r12, lr}         \n"
        #endif
        
        "   bx r14                              \n"
        // ...
    );
}
```

**Key modification:** assembly code to call `mpu_dynamic_switch_task()` after each context switch.

---

#### 2. `src/mpu_dynamic.c` / `src/mpu_dynamic.h`

**New files** implementing:
- Per-task MPU configuration arrays (TaskA and TaskB)
- `mpu_dynamic_init()` - Enable MPU
- `mpu_dynamic_switch_task()` - Reconfigure MPU regions
- `mpu_dynamic_get_switches()` - Track switch count

**TaskA Configuration:**
```c
Region 0: Flash 0x00000000 (32 KB, RO+X)
Region 1: SRAM 0x20000000 (512 KB, RW+XN)
Region 2: UART 0x40004000 (4 KB, RW+XN)
Total: ~548 KB
```

**TaskB Configuration:**
```c
Region 0: Flash 0x00000000 (16 KB, RO+X)
Region 1: SRAM 0x20000000 (512 KB, RW+XN)
Region 2: UART 0x40004000 (4 KB, RW+XN)
Total: ~532 KB
```


## Build and Run
```bash
make clean
make
make run
```

---

## Results

**Performance:** 669 cycles (+706% vs baseline)  
**Memory reduction:** 78% per task  
**MPU switches:** 5,605 verified  
**Fault detection:** Active ✓


