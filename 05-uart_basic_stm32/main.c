#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "log.h"

#include "uart.h"

#define TASK_NAME                       "task"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

#define UART_RECV_TASK_NAME             "recv_task"
#define UART_RECV_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define UART_RECV_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)

#define UART_SEND_TASK_NAME             "send_task"
#define UART_SEND_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define UART_SEND_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)

#define UART_BAUDRATE                   115200
#define RX_BUFFER_SIZE                  16

static void task_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                vTaskDelay(5000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                size_t free_heap = xPortGetFreeHeapSize();
                configPRINTF("hello: tick = %lu, free_heap = %lu\r\n", current_tick, free_heap);
        }
}

static void uart_recv_service(void *pvParameters)
{
        uint8_t buf[RX_BUFFER_SIZE];

        LOG_I("started");

        while(1) {
                memset(buf, 0, sizeof(buf));
                uart_read(buf, sizeof(buf) - 1, portMAX_DELAY);
                LOG_I("received: <%s>", (char*)buf);
        }
}

static void uart_send_service(void *pvParameters)
{
        char ch = '!';

        LOG_I("started");

        TickType_t last_tick = xTaskGetTickCount();
        while(1) {
                vTaskDelayUntil(&last_tick, 1000UL / portTICK_RATE_MS);
                uart_write((uint8_t*)&ch, 1, portMAX_DELAY);
                if (++ch > '~')
                        ch = '!';
        }
}

int main(void)
{
        BaseType_t s;
        
        s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(uart_recv_service, UART_RECV_TASK_NAME, UART_RECV_TASK_STACK_SIZE, NULL, UART_RECV_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(uart_send_service, UART_SEND_TASK_NAME, UART_SEND_TASK_STACK_SIZE, NULL, UART_SEND_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        uart_init(UART_BAUDRATE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
