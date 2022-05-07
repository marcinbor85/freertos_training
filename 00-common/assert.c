#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

void utils_rtos_assert(const char * const pcFileName, unsigned long ulLine)
{
        taskENTER_CRITICAL();

        fprintf(stderr, "ASSERT: %s:%lu\n", pcFileName, ulLine);
        exit(-1);
}
