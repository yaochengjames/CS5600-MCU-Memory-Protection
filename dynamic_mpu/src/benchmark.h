#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

void benchmark_init(void);
uint32_t benchmark_get_cycles(void);
void benchmark_reset(void);

#endif

