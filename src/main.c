#include "FreeRTOS.h"
#include "task.h"

/* Simple demo tasks */

void TaskA(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        /* Artificial delay */
        for (volatile int i = 0; i < 100000; i++);
    }
}

void TaskB(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        /* Artificial delay */
        for (volatile int i = 0; i < 200000; i++);
    }
}

int main(void) {

    /* Create tasks */
    xTaskCreate(TaskA, "TaskA", 128, NULL, 1, NULL);
    xTaskCreate(TaskB, "TaskB", 128, NULL, 1, NULL);

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    for (;;);
}

