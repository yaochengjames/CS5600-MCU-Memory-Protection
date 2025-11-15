#include "benchmark.h"

// ARM Cortex-M4 DWT (Data Watchpoint and Trace) registers
#define DWT_CTRL    (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT  (*(volatile uint32_t*)0xE0001004)
#define DEM_CR      (*(volatile uint32_t*)0xE000EDFC)

#define DEM_CR_TRCENA (1 << 24)
#define DWT_CTRL_CYCCNTENA (1 << 0)

void benchmark_init(void)
{
    // Enable trace
    DEM_CR |= DEM_CR_TRCENA;
    
    // Reset cycle counter
    DWT_CYCCNT = 0;
    
    // Enable cycle counter
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;
}

uint32_t benchmark_get_cycles(void)
{
    return DWT_CYCCNT;
}

void benchmark_reset(void)
{
    DWT_CYCCNT = 0;
}
