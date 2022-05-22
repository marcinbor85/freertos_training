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

#include "bsp/bsp.h"

#define TASK_NAME                       "task"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

#define LED_TASK_NAME                   "led_task"
#define LED_TASK_PRIORITY               (tskIDLE_PRIORITY + 1)
#define LED_TASK_STACK_SIZE             (configMINIMAL_STACK_SIZE)

#define BUTTON_TASK_NAME                "button_task"
#define BUTTON_TASK_PRIORITY            (tskIDLE_PRIORITY + 1)
#define BUTTON_TASK_STACK_SIZE          (configMINIMAL_STACK_SIZE)

static void task_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                vTaskDelay(5000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                size_t free_heap = xPortGetFreeHeapSize();
                LOG_T("tick = %lu, free_heap = %lu", current_tick, free_heap);
        }
}

static void led_service(void *pvParameters)
{
        LOG_I("started");

        bool state = false;
        bsp_led_set_state(state);

        TickType_t last_tick = xTaskGetTickCount();
        while(1) {
                vTaskDelayUntil(&last_tick, 1000UL / portTICK_RATE_MS);

                state = !state;
                bsp_led_set_state(state);
        }
}

static void button_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                bsp_button_event_t event = bsp_button_wait(portMAX_DELAY);
                switch (event) {
                case BSP_BUTTON_EVENT_PRESSED:
                        LOG_I("button PRESSED");
                        break;
                case BSP_BUTTON_EVENT_RELEASED:
                        LOG_I("button RELEASED");
                        break;
                case BSP_BUTTON_EVENT_TIMEOUT:
                default:
                        break;
                }
        }
}

int main(void)
{
        BaseType_t s;

        bsp_init();

        s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(led_service, LED_TASK_NAME, LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(button_service, BUTTON_TASK_NAME, BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
