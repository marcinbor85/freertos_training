#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"

void utils_rtos_printf(const char *pcFormat, ...)
{
        taskENTER_CRITICAL();

        va_list arg;

        va_start( arg, pcFormat );
        vprintf( pcFormat, arg );
        va_end( arg );

        taskEXIT_CRITICAL();
}
