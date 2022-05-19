/*
MIT License

Copyright (c) 2022 Marcin Borowicz <marcinbor85@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

#define UART_RECV_TASK_NAME             "recv_task"
#define UART_RECV_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define UART_RECV_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)

#define UART_SEND_TASK_NAME             "send_task"
#define UART_SEND_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define UART_SEND_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)

#if defined(TARGET_ARCH_LINUX)
#define UART_PORT                       "/dev/ttyUSB0"
#else
#define UART_PORT                       "uart3"
#endif

#define UART_BAUDRATE                   115200
#define RX_BUFFER_SIZE                  16

#define UART_TX_BUF_SIZE                64
#define UART_RX_BUF_SIZE                64

static struct uart *g_uart;

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
                uart_read(g_uart, buf, sizeof(buf) - 1, portMAX_DELAY);
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
                uart_write(g_uart, (uint8_t*)&ch, 1, portMAX_DELAY);
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

        g_uart = uart_open(UART_PORT, UART_BAUDRATE, UART_TX_BUF_SIZE, UART_RX_BUF_SIZE);
        configASSERT(g_uart != NULL);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
