#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system/log.h"

void vApplicationMallocFailedHook(void)
{
        configASSERT(pdFALSE);
        exit(-1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
        if (pcTaskName == NULL)
                pcTaskName = "NULL";
        LOG_F("task: <%s>", pcTaskName);
        configASSERT(pdFALSE);
        exit(-1);
}
