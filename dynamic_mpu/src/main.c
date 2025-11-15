#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* ----- CMSDK UART0 base for mps2-an386 ----- */
#define UART0_BASE     0x40004000
#define UART0_DATA     (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_STATE    (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART0_CTRL     (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART0_BAUDDIV  (*(volatile uint32_t *)(UART0_BASE + 0x0C))

/* STATE bit0 = TX_BUSY */
#define UART_TX_BUSY   (1 << 0)

/* UART init for CMSDK UART */
void uart_init(void)
{
    UART0_CTRL = 0x1;   // enable TX only (RX not needed)
    UART0_BAUDDIV = 16; // BAUDDIV default, QEMU accepts almost anything
}

void uart_putc(char c)
{
    while (UART0_STATE & UART_TX_BUSY);  // Wait until TX not busy

    UART0_DATA = c;
}

void uart_print(const char *s)
{
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void TaskA(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        uart_print("Task A running\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void TaskB(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        uart_print("Task B running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    uart_init();
    uart_print("=== FreeRTOS + CMSDK UART OK ===\n");

    xTaskCreate(TaskA, "TaskA", 256, NULL, 1, NULL);
    xTaskCreate(TaskB, "TaskB", 256, NULL, 1, NULL);

    uart_print("Starting scheduler...\n");

    vTaskStartScheduler();

    for (;;) {}
}

