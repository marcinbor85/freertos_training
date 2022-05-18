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

#ifndef _UTILS_NODE_H
#define _UTILS_NODE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

struct node;
struct node_message;
struct node_control;

typedef void (*node_message_handler_t)(struct node *self, struct node_message *message);
typedef void (*node_control_handler_t)(struct node *self, struct node_control *control);

typedef enum {
        NODE_CONTROL_TYPE_START,
        NODE_CONTROL_TYPE_NOTIFY,
        NODE_CONTROL_TYPE_TIMEOUT,
} node_control_t;

struct node_control {
        node_control_t type;

        void *payload;
};

struct node_message {
        struct node *dest;
        struct node *source;

        uint32_t id;
        void *payload;
};

struct node_descriptor {
        const char *name;
        uint32_t stack_size;
        UBaseType_t priority;
        UBaseType_t message_queue_size;
        TickType_t wakeup_period;

        node_control_handler_t control_handler;
        node_message_handler_t message_handler;
};

struct node {
        struct node_descriptor const *desc;

        TaskHandle_t task;
        QueueHandle_t queue;
        TickType_t last_wakeup;

        void *context;
};

struct node* node_create(const struct node_descriptor *desc, void *context);
BaseType_t node_send_message(struct node *dest, struct node *source, BaseType_t id, void *payload, BaseType_t timeout);
BaseType_t node_notify(struct node *dest, void *payload, BaseType_t timeout);

#endif /* _UTILS_NODE_H */
