#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system/log.h"

#include "hal/uart.h"

#define TASK_NAME                       "task"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

#define UART_ECHO_TASK_NAME             "uart_echo_task"
#define UART_ECHO_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define UART_ECHO_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)

#define UART_BAUDRATE                   115200
#define ECHO_BUFFER_SIZE                16

static void task_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                vTaskDelay(1000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                size_t free_heap = xPortGetFreeHeapSize();
                configPRINTF("hello: tick = %lu, free_heap = %lu\r\n", current_tick, free_heap);
        }
}

static void uart_echo_service(void *pvParameters)
{
        uint8_t buf[ECHO_BUFFER_SIZE];
        size_t rx_size;
        size_t tx_size;

        LOG_I("started");

        while(1) {
                rx_size = uart_read(buf, sizeof(buf), portMAX_DELAY);
                tx_size = 0;
                while (tx_size < rx_size)
                        tx_size += uart_write(&buf[tx_size], rx_size - tx_size, portMAX_DELAY);
        }
}

int main(void)
{
        BaseType_t s;
        
        s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(uart_echo_service, UART_ECHO_TASK_NAME, UART_ECHO_TASK_STACK_SIZE, NULL, UART_ECHO_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        uart_init(UART_BAUDRATE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
