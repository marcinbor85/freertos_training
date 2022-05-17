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

#include "utils/worker.h"

#define TASK_NAME                       "task"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

#define WORKER_NAME                     "worker"
#define WORKER_PRIORITY                 (tskIDLE_PRIORITY + 1)
#define WORKER_STACK_SIZE               (configMINIMAL_STACK_SIZE)
#define WORKER_QUEUE_SIZE               1

static struct worker_manager *g_worker;

static void worker_callback(struct worker_manager *worker, struct worker_item *item)
{
        TickType_t now = xTaskGetTickCount();
        
        TickType_t real_diff = now - item->start_time;
        if (real_diff > item->delay) {
                TickType_t late = real_diff - item->delay;
                LOG_W("late: %ld", late);
        }
        
        LOG_I("context = %ld, item_context = %ld", (long)worker->context, (long)item->context);
        vTaskDelay(50UL / portTICK_RATE_MS);
}

static void task_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                // vTaskDelay(1000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                size_t free_heap = xPortGetFreeHeapSize();
                LOG_T("tick = %lu, free_heap = %lu", current_tick, free_heap);

                struct worker_item *item1 = worker_call_after(g_worker, 300UL / portTICK_RATE_MS, worker_callback, (void*)1);
                struct worker_item *item2 = worker_call_after(g_worker, 100UL / portTICK_RATE_MS, worker_callback, (void*)2);
                struct worker_item *item3 = worker_call_after(g_worker, 200UL / portTICK_RATE_MS, worker_callback, (void*)3);

                (void)item1;
                (void)item2;
                (void)item3;
                
                //worker_cancel(g_worker, item3);
        }
}

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        g_worker = worker_create(WORKER_NAME, WORKER_STACK_SIZE, WORKER_PRIORITY, WORKER_QUEUE_SIZE, (void*)123);
        configASSERT(g_worker != NULL);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
