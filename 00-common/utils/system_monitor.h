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

#ifndef _UTILS_SYSTEM_MONITOR_H
#define _UTILS_SYSTEM_MONITOR_H

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

struct system_monitor_task;
struct system_monitor;

typedef void (*system_monitor_expired_callback_t)(struct system_monitor *self, struct system_monitor_task *task);

struct system_monitor_task {
        struct system_monitor_task *next;

        char const *name;
        TickType_t last_update;
        TickType_t permitted_delay;
};

struct system_monitor {
        struct system_monitor_task *tasks;
        TaskHandle_t task;
        QueueHandle_t queue;
        TickType_t check_period;
        TickType_t last_check;
        system_monitor_expired_callback_t expired_callback;
};

struct system_monitor* system_monitor_create(const char *name, uint32_t stack_size, UBaseType_t priority, UBaseType_t cmd_queue_size, TickType_t check_period, system_monitor_expired_callback_t expired_callback);
struct system_monitor_task* system_monitor_register_task(struct system_monitor *self, const char *name, TickType_t permitted_delay);
void system_monitor_unregister_task(struct system_monitor *self, struct system_monitor_task *task);
void system_monitor_update(struct system_monitor *self, struct system_monitor_task *task);

#endif /* _UTILS_SYSTEM_MONITOR_H */
