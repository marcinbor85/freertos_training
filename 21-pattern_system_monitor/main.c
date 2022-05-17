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
#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system/log.h"

#include "utils/system_monitor.h"

#define TASK_1_NAME                     "task_bad"
#define TASK_2_NAME                     "task_good"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

#define SYSTEM_MONITOR_NAME             "monitor"
#define SYSTEM_MONITOR_PRIORITY         (tskIDLE_PRIORITY + 1)
#define SYSTEM_MONITOR_STACK_SIZE       (configMINIMAL_STACK_SIZE)
#define SYSTEM_MONITOR_QUEUE_SIZE       1
#define SYSTEM_MONITOR_CHECK_PERIOD     (1000 / portTICK_RATE_MS)

static struct system_monitor *g_monitor;

static void expired_callback(struct system_monitor *worker, struct system_monitor_task *task)
{
        LOG_F("system monitor time expired for <%s>", task->name);
        configASSERT(false);
}

static void task_service(void *pvParameters)
{
        long max_cntr = (long)pvParameters;
        long cntr = 0;
        const char *name = pcTaskGetName(NULL);

        LOG_I("started: <%s>", name);

        struct system_monitor_task* task = system_monitor_register_task(g_monitor, name, 5000UL / portTICK_RATE_MS);

        while(1) {
                vTaskDelay(1000UL / portTICK_RATE_MS);

                if (cntr < max_cntr) {
                        cntr++;
                        system_monitor_update(g_monitor, task);
                        LOG_I("update task: <%s>", name);
                }
        }
}

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_service, TASK_1_NAME, TASK_STACK_SIZE, (void*)5, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);
        s = xTaskCreate(task_service, TASK_2_NAME, TASK_STACK_SIZE, (void*)20, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        g_monitor = system_monitor_create(
                SYSTEM_MONITOR_NAME,
                SYSTEM_MONITOR_STACK_SIZE,
                SYSTEM_MONITOR_PRIORITY,
                SYSTEM_MONITOR_QUEUE_SIZE,
                SYSTEM_MONITOR_CHECK_PERIOD,
                expired_callback
        );
        configASSERT(g_monitor != NULL);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
