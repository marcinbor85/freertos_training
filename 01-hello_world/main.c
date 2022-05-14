#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system/shell.h"

#define TASK_NAME               "task"
#define TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE)

static void task_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(1000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                configPRINTF("hello: tick = %lu" SHELL_NEW_LINE, current_tick);
        }
}

int main(void)
{
        BaseType_t s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
