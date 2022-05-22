#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook(void)
{
        configASSERT(pdFALSE);
        exit(-1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
        (void)xTask;
        (void)pcTaskName;
        
        configASSERT(pdFALSE);
        exit(-1);
}
