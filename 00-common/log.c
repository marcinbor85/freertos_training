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

#include "log.h"
#include "shell.h"
#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void log_printf(int level, const char *file, const char *func, unsigned long line, const char *fmt, ... )
{
        char level_name[8] = {0};
        char prefix[8] = {0};

        utils_rtos_stdout_lock();

        if (level <= LOGGING_LEVEL_ERROR) {
                strcpy(level_name, "ERROR");
                strcpy(prefix, SHELL_FONT_RED);
        } else if (level <= LOGGING_LEVEL_WARNING) {
                strcpy(level_name, "WARNING");
                strcpy(prefix, SHELL_FONT_YELLOW);
        } else if (level <= LOGGING_LEVEL_INFO) {
                strcpy(level_name, "INFO");
                strcpy(prefix, SHELL_FONT_GREEN);
        } else {
                strcpy(level_name, "DEBUG");
                strcpy(prefix, SHELL_FONT_RESET);
        }
        
        TickType_t ticks = xTaskGetTickCount();
        printf("%s%08lu %s %s@%s:%lu ", prefix, ticks, level_name, func, file, line);

        va_list arg;
        va_start( arg, fmt );
        vprintf( fmt, arg );
        va_end( arg );

        printf(SHELL_FONT_RESET SHELL_NEW_LINE);

        utils_rtos_stdout_unlock();
}
