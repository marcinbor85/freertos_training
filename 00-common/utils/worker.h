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

#ifndef _UTILS_WORKER_H
#define _UTILS_WORKER_H

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

struct worker_manager;
struct worker_item;

typedef void (*worker_callback_t)(struct worker_manager *self, struct worker_item *item);

struct worker_item {
        struct worker_item *next;

        worker_callback_t callback;
        TickType_t start_time;
        TickType_t delay;
        void *context;
};

struct worker_manager {
        struct worker_item *items;
        TaskHandle_t task;
        QueueHandle_t queue;
        void *context;
};

struct worker_manager* worker_create(const char *name, uint32_t stack_size, UBaseType_t priority, UBaseType_t cmd_queue_size, void *context);
struct worker_item* worker_call_after(struct worker_manager *self, TickType_t delay, worker_callback_t callback, void *context);
void worker_cancel(struct worker_manager *self, struct worker_item *item);

#endif /* _UTILS_WORKER_H */
