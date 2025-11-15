#include "uart.h"
#include <stdio.h>

#define UART0_BASE     0x40004000
#define UART0_DATA     (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_STATE    (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART0_CTRL     (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART0_BAUDDIV  (*(volatile uint32_t *)(UART0_BASE + 0x0C))
#define UART_TX_BUSY   (1 << 0)

void uart_init(void)
{
    UART0_CTRL = 0x1;
    UART0_BAUDDIV = 16;
}

void uart_putc(char c)
{
    while (UART0_STATE & UART_TX_BUSY);
    UART0_DATA = c;
}

void uart_print(const char *s)
{
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void uart_printf(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    uart_print(buf);
}
