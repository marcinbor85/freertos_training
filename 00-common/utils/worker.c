#include "worker.h"

#include <stdbool.h>

typedef enum {
        WORKER_CMD_TYPE_CALL_AFTER,
        WORKER_CMD_TYPE_CANCEL,
} worker_cmd_type_t;

struct worker_cmd {
        worker_cmd_type_t type;
        struct worker_item *item;
};

static TickType_t get_remaining_time(struct worker_item *item, TickType_t now)
{
        TickType_t ret;

        if (now - item->start_time >= item->delay) {
                ret = 0;
        } else {
                ret =  item->delay + item->start_time - now;
        }

        return ret;
}

static void add_worker_item(struct worker_manager *self, struct worker_item *item)
{
        struct worker_item *next_item = self->items;
        struct worker_item *prev_item = NULL;

        TickType_t now = xTaskGetTickCount();

        if (next_item == NULL) {
                // insert at the beginning
                item->next = NULL;
                self->items = item;
                return;
        }

        TickType_t remaining_time = get_remaining_time(item, now);

        while (next_item != NULL) {
                TickType_t next_item_remaining_time = get_remaining_time(next_item, now);

                if (remaining_time < next_item_remaining_time) {
                        // insert before next item
                        item->next = next_item;
                        if (prev_item == NULL) {
                                self->items = item;
                        } else {
                                prev_item->next = item;
                        }
                        break;
                } else {
                        if (next_item->next == NULL) {
                                // insert after current item
                                item->next = next_item->next;
                                next_item->next = item;
                                break;
                        }
                }

                prev_item = next_item;
                next_item = next_item->next;
        }
}

static void remove_worker_item(struct worker_manager *self, struct worker_item *item)
{
        struct worker_item *next_item = self->items;
        struct worker_item *prev_item = NULL;

        while (next_item != NULL) {
                if (item == next_item) {
                        if (prev_item == NULL) {
                                self->items = next_item->next;
                        } else {
                                prev_item->next = next_item->next;
                        }
                        vPortFree(next_item);
                        break;
                }

                prev_item = next_item;
                next_item = next_item->next;
        }
}

static void worker_task(void *params)
{
        struct worker_manager *self = (struct worker_manager *)params;
        struct worker_cmd cmd;
        TickType_t timeout;
        BaseType_t s;

        while (1) {
                TickType_t now = xTaskGetTickCount();
                struct worker_item *current_item = self->items;

                timeout = (current_item == NULL) ? portMAX_DELAY : get_remaining_time(current_item, now);

                s = xQueueReceive(self->queue, &cmd, timeout);
                if (s == pdFALSE) {
                        current_item->callback(self, current_item);
                        self->items = current_item->next;
                        vPortFree(current_item);
                } else {
                        switch (cmd.type) {
                        case WORKER_CMD_TYPE_CALL_AFTER:
                                add_worker_item(self, cmd.item);
                                break;
                        case WORKER_CMD_TYPE_CANCEL:
                                remove_worker_item(self, cmd.item);
                                break;
                        default:
                                break;
                        }
                }
        }
}

struct worker_manager* worker_create(const char *name, uint32_t stack_size, UBaseType_t priority, UBaseType_t cmd_queue_size, void *context)
{
        struct worker_manager *self = pvPortMalloc(sizeof(struct worker_manager));
        configASSERT(self != NULL);

        self->items = NULL;
        self->context = context;

        BaseType_t s;
        
        s = xTaskCreate(worker_task, name, stack_size, self, priority, &self->task);     
        configASSERT(s != pdFALSE);

        self->queue = xQueueCreate(cmd_queue_size, sizeof(struct worker_cmd));
        configASSERT(self->queue != NULL);

        return self;
}

struct worker_item* worker_call_after(struct worker_manager *self, TickType_t delay, worker_callback_t callback, void *context)
{
        struct worker_item *item = pvPortMalloc(sizeof(struct worker_item));
        configASSERT(item != NULL);

        if (item == NULL)
                return NULL;

        item->next = NULL;
        item->callback = callback;
        item->context = context;
        item->delay = delay;
        item->start_time = xTaskGetTickCount();

        struct worker_cmd cmd = {
                .type = WORKER_CMD_TYPE_CALL_AFTER,
                .item = item,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);

        return item;
}

void worker_cancel(struct worker_manager *self, struct worker_item *item)
{
        struct worker_cmd cmd = {
                .type = WORKER_CMD_TYPE_CANCEL,
                .item = item,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);
}
