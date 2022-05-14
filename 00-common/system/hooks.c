#include <stdlib.h>

#include "FreeRTOS.h"

void vApplicationMallocFailedHook(void)
{
        configASSERT(pdFALSE);
        exit(-1);
}

void vApplicationStackOverflowHook(void)
{
        configASSERT(pdFALSE);
        exit(-1);
}
