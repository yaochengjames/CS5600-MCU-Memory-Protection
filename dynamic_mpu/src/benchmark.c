#include "benchmark.h"

// SysTick registers
#define SYST_CSR   (*(volatile uint32_t*)0xE000E010)  // Control and Status
#define SYST_RVR   (*(volatile uint32_t*)0xE000E014)  // Reload Value
#define SYST_CVR   (*(volatile uint32_t*)0xE000E018)  // Current Value

#define SYST_CSR_ENABLE    (1 << 0)
#define SYST_CSR_CLKSOURCE (1 << 2)  // Use processor clock

static uint32_t systick_reload_value = 0;

void benchmark_init(void)
{
    // Disable SysTick
    SYST_CSR = 0;
    
    // Set reload value to maximum (24-bit)
    systick_reload_value = 0x00FFFFFF;
    SYST_RVR = systick_reload_value;
    
    // Clear current value
    SYST_CVR = 0;
    
    // Enable SysTick with processor clock, no interrupt
    SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_CLKSOURCE;
}

uint32_t benchmark_get_cycles(void)
{
    // SysTick counts DOWN, so we need to invert it
    // to get an increasing counter
    return systick_reload_value - SYST_CVR;
}

void benchmark_reset(void)
{
    SYST_CVR = 0;
}
