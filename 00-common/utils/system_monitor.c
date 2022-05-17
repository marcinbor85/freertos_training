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

#include "system_monitor.h"

#include <stdbool.h>

typedef enum {
        SYSTEM_MONITOR_CMD_REGISTER,
        SYSTEM_MONITOR_CMD_UNREGISTER,
        SYSTEM_MONITOR_CMD_UPDATE,
} system_monitor_cmd_t;

struct system_monitor_cmd {
        system_monitor_cmd_t type;
        struct system_monitor_task *task;
};

static void check_all_tasks(struct system_monitor_manager *self)
{
        struct system_monitor_task *task = self->tasks;

        TickType_t now = xTaskGetTickCount();

        while (task != NULL) {
                if (now - task->last_update >= task->permitted_delay) {
                        if (self->expired_callback != NULL) {
                                self->expired_callback(self, task);
                        }
                        configASSERT(false);
                }
                task = task->next;
        }
}

static void add_task(struct system_monitor_manager *self, struct system_monitor_task *task)
{
        task->next = self->tasks;
        self->tasks = task;
}

static void remove_task(struct system_monitor_manager *self, struct system_monitor_task *task)
{
        struct system_monitor_task *next_task = self->tasks;
        struct system_monitor_task *prev_task = NULL;

        while (next_task != NULL) {
                if (next_task == task) {
                        if (prev_task == NULL) {
                                self->tasks = next_task->next;
                        } else {
                                prev_task->next = next_task->next;
                        }
                        vPortFree(next_task);
                        break;
                }

                prev_task = next_task;
                next_task = next_task->next;
        }
}

static void update_task(struct system_monitor_manager *self, struct system_monitor_task *task)
{
        TickType_t now = xTaskGetTickCount();

        task->last_update = now;
}

static void system_monitor_service_task(void *params)
{
        struct system_monitor_manager *self = (struct system_monitor_manager *)params;
        struct system_monitor_cmd cmd;
        TickType_t timeout;

        BaseType_t s;

        while (1) {
                TickType_t now = xTaskGetTickCount();
                TickType_t diff = now - self->last_check;
                timeout = (diff >= self->check_period) ? 0 : self->check_period - diff;

                s = xQueueReceive(self->queue, &cmd, timeout);
                if (s == pdFALSE) {
                        self->last_check = xTaskGetTickCount();
                        check_all_tasks(self);
                } else {
                        switch (cmd.type) {
                        case SYSTEM_MONITOR_CMD_REGISTER:
                                add_task(self, cmd.task);
                                break;
                        case SYSTEM_MONITOR_CMD_UNREGISTER:
                                remove_task(self, cmd.task);
                                break;
                        case SYSTEM_MONITOR_CMD_UPDATE:
                                update_task(self, cmd.task);
                                break;
                        default:
                                configASSERT(false);
                                break;
                        }
                }
        }
}

struct system_monitor_manager* system_monitor_create(const char *name, uint32_t stack_size, UBaseType_t priority, UBaseType_t cmd_queue_size, TickType_t check_period, system_monitor_expired_callback_t expired_callback)
{
        struct system_monitor_manager *self = pvPortMalloc(sizeof(struct system_monitor_manager));
        configASSERT(self != NULL);

        self->tasks = NULL;
        self->expired_callback = expired_callback;
        self->check_period = check_period;
        self->last_check = xTaskGetTickCount();

        BaseType_t s;
        
        s = xTaskCreate(system_monitor_service_task, name, stack_size, self, priority, &self->task);     
        configASSERT(s != pdFALSE);

        self->queue = xQueueCreate(cmd_queue_size, sizeof(struct system_monitor_cmd));
        configASSERT(self->queue != NULL);

        return self;
}

struct system_monitor_task* system_monitor_register_task(struct system_monitor_manager *self, const char *name, TickType_t permitted_delay)
{
        struct system_monitor_task *task = pvPortMalloc(sizeof(struct system_monitor_task));
        configASSERT(task != NULL);

        if (task == NULL)
                return NULL;

        task->next = NULL;
        task->name = name;
        task->permitted_delay = permitted_delay;
        task->last_update = xTaskGetTickCount();

        struct system_monitor_cmd cmd = {
                .type = SYSTEM_MONITOR_CMD_REGISTER,
                .task = task,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);

        return task;
}

void system_monitor_unregister_task(struct system_monitor_manager *self, struct system_monitor_task *task)
{
        struct system_monitor_cmd cmd = {
                .type = SYSTEM_MONITOR_CMD_UNREGISTER,
                .task = task,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);
}

void system_monitor_update(struct system_monitor_manager *self, struct system_monitor_task *task)
{
        struct system_monitor_cmd cmd = {
                .type = SYSTEM_MONITOR_CMD_UPDATE,
                .task = task,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);
}
