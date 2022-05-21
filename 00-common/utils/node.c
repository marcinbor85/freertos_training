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

#include "node.h"

typedef enum {
        NODE_QUEUE_ITEM_TYPE_CONTROL,
        NODE_QUEUE_ITEM_TYPE_MESSAGE,
} node_queue_item_t;

struct node_queue_item {
        node_queue_item_t type;
        union {
            struct node_control control;
            struct node_message message;    
        };
};

static void service_task(void *params)
{
        struct node *self = (struct node*)params;
        struct node_queue_item item;
        TickType_t timeout;
        struct node_control control;

        if (self->desc->control_handler != NULL) {
                control.type = NODE_CONTROL_TYPE_START;
                control.payload = NULL;
                self->desc->control_handler(self, &control);
        }
        
        while (1) {
                if (self->desc->wakeup_period != portMAX_DELAY) {
                        TickType_t now = xTaskGetTickCount();
                        TickType_t diff = now - self->last_wakeup;
                        timeout = (diff >= self->desc->wakeup_period) ? 0 : self->desc->wakeup_period - diff;
                } else {
                        timeout = portMAX_DELAY;
                }

                BaseType_t s = xQueueReceive(self->queue, &item, timeout);
                if (s == pdFALSE) {
                        self->last_wakeup = xTaskGetTickCount();
                        if (self->desc->control_handler != NULL) {
                                control.type = NODE_CONTROL_TYPE_TIMEOUT;
                                control.payload = NULL;
                                self->desc->control_handler(self, &control);
                        }
                } else {
                        switch (item.type) {
                        case NODE_QUEUE_ITEM_TYPE_CONTROL:
                                if (self->desc->control_handler != NULL)
                                        self->desc->control_handler(self, &item.control);
                                break;
                        case NODE_QUEUE_ITEM_TYPE_MESSAGE:
                                if (self->desc->message_handler != NULL)
                                        self->desc->message_handler(self, &item.message);
                                break;
                        default:
                                configASSERT(false);
                        }
                }
        }
}

struct node* node_create(const struct node_descriptor *desc, void *context)
{
        struct node *self = pvPortMalloc(sizeof(struct node));
        configASSERT(self != NULL);

        self->desc = desc;
        self->context = context;

        BaseType_t s;
        
        self->queue = xQueueCreate(self->desc->message_queue_size, sizeof(struct node_queue_item));
        configASSERT(self->queue != NULL);

        s = xTaskCreate(service_task, self->desc->name, self->desc->stack_size, self, self->desc->priority, &self->task);     
        configASSERT(s != pdFALSE);

        return self;
}

BaseType_t node_send_message(struct node *dest, struct node *source, BaseType_t id, void *payload, BaseType_t timeout)
{
        struct node_queue_item item = {
                .type = NODE_QUEUE_ITEM_TYPE_MESSAGE,
                .message = {
                        .dest = dest,
                        .source = source,
                        .id = id,
                        .payload = payload,
                }
        };

        BaseType_t s = xQueueSend(dest->queue, &item, timeout);
        return s;
}

BaseType_t node_notify(struct node *dest, void *payload, BaseType_t timeout)
{
        struct node_queue_item item = {
                .type = NODE_QUEUE_ITEM_TYPE_CONTROL,
                .control = {
                        .type = NODE_CONTROL_TYPE_NOTIFY,
                        .payload = payload,
                }
        };

        BaseType_t s = xQueueSend(dest->queue, &item, timeout);
        return s;
}
