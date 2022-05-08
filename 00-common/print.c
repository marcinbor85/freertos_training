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

#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "semphr.h"

static xSemaphoreHandle g_mutex;

static void mutex_lazy_init(void)
{
        if (g_mutex != NULL)
                return;
        g_mutex = xSemaphoreCreateMutex();
}

void utils_rtos_printf(const char *fmt, ...)
{
        utils_rtos_stdout_lock();

        va_list arg;

        va_start( arg, fmt );
        vprintf( fmt, arg );
        va_end( arg );

        utils_rtos_stdout_unlock();
}

void utils_rtos_stdout_lock(void)
{
        mutex_lazy_init();
        xSemaphoreTake(g_mutex, portMAX_DELAY);
}

void utils_rtos_stdout_unlock(void)
{
        mutex_lazy_init();
        xSemaphoreGive(g_mutex);
}
