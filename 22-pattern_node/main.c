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

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system/log.h"

#include "utils/node.h"

#define TASK_1_NAME                     "task_server"
#define TASK_2_NAME                     "task_client_1"
#define TASK_3_NAME                     "task_client_2"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

static void server_control_handler(struct node *self, node_control_event_t event);
static void server_message_handler(struct node *self, struct node_message *message);

static void client_control_handler(struct node *self, node_control_event_t event);
static void client_message_handler(struct node *self, struct node_message *message);

static struct node_descriptor g_node_server_desc = {
        .name                   = "server",
        .stack_size             = configMINIMAL_STACK_SIZE,
        .priority               = tskIDLE_PRIORITY + 1,
        .message_queue_size     = 4,
        .wakeup_period          = portMAX_DELAY,
        .control_handler        = server_control_handler,
        .message_handler        = server_message_handler,
};

static struct node_descriptor g_node_client_1_desc = {
        .name                   = "client_1",
        .stack_size             = configMINIMAL_STACK_SIZE,
        .priority               = tskIDLE_PRIORITY + 1,
        .message_queue_size     = 4,
        .wakeup_period          = 500UL / portTICK_RATE_MS,
        .control_handler        = client_control_handler,
        .message_handler        = client_message_handler,
};

static struct node_descriptor g_node_client_2_desc = {
        .name                   = "client_2",
        .stack_size             = configMINIMAL_STACK_SIZE,
        .priority               = tskIDLE_PRIORITY + 1,
        .message_queue_size     = 4,
        .wakeup_period          = 1000UL / portTICK_RATE_MS,
        .control_handler        = client_control_handler,
        .message_handler        = client_message_handler,
};

static struct node *g_node_server;
static struct node *g_node_client_1;
static struct node *g_node_client_2;

typedef enum {
        ID_SERVER_MESSAGE_SUM_REQUEST   = 0x00000001,
        ID_SERVER_MESSAGE_MUL_REQUEST   = 0x00000002,
        ID_CLIENT_MESSAGE_RESPONSE      = 0x00010000,
} message_id;

struct request_data {
        long long comp1;
        long long comp2;
};

struct response_data {
        long long sum;
};

struct client_context {
        long long prev_val;
        long long next_val;
        message_id req_type;
};

static struct client_context g_node_client_1_context = {0, 1, ID_SERVER_MESSAGE_SUM_REQUEST};
static struct client_context g_node_client_2_context = {2, 1, ID_SERVER_MESSAGE_MUL_REQUEST};

static void server_control_handler(struct node *self, node_control_event_t event)
{
        switch (event) {
        case NODE_CONTROL_EVENT_START: {
                LOG_I("started <%s>", self->desc->name);
                break;
        }
        case NODE_CONTROL_EVENT_TIMEOUT:
                break;
        default:
                LOG_W("unsupported event <%lu> for <%s>", event, self->desc->name);
                break;
        }
}

static void server_message_handler(struct node *self, struct node_message *message)
{
        switch (message->id) {
        case ID_SERVER_MESSAGE_SUM_REQUEST: {
                struct request_data *req = (struct request_data *)message->payload;
                long sum = req->comp1 + req->comp2;
                vPortFree(message->payload);

                LOG_I("request from <%s>: %lld + %lld", message->source->desc->name, req->comp1, req->comp2);

                struct response_data *resp = pvPortMalloc(sizeof(struct response_data));
                resp->sum = sum;
                node_send_message(message->source, self, ID_CLIENT_MESSAGE_RESPONSE, resp, portMAX_DELAY);
                break;
        }
        case ID_SERVER_MESSAGE_MUL_REQUEST: {
                struct request_data *req = (struct request_data *)message->payload;
                long sum = req->comp1 * req->comp2;
                vPortFree(message->payload);

                LOG_I("request from <%s>: %lld * %lld", message->source->desc->name, req->comp1, req->comp2);

                struct response_data *resp = pvPortMalloc(sizeof(struct response_data));
                resp->sum = sum;
                node_send_message(message->source, self, ID_CLIENT_MESSAGE_RESPONSE, resp, portMAX_DELAY);
                break;
        }
        default:
                LOG_W("unsupported message id <%lu> for <%s>", message->id, self->desc->name);
                break;
        }
}

static void client_control_handler(struct node *self, node_control_event_t event)
{
        switch (event) {
        case NODE_CONTROL_EVENT_START: {
                LOG_I("started <%s>", self->desc->name);
                break;
        }
        case NODE_CONTROL_EVENT_TIMEOUT: {
                struct client_context *context = (struct client_context *)self->context;
                struct request_data *req = pvPortMalloc(sizeof(struct request_data));
                req->comp1 = context->prev_val;
                req->comp2 = context->next_val;
                node_send_message(g_node_server, self, context->req_type, req, portMAX_DELAY);
                break;
        }
        default:
                LOG_W("unsupported event <%lu> for <%s>", event, self->desc->name);
                break;
        }
}

static void client_message_handler(struct node *self, struct node_message *message)
{
        switch (message->id) {
        case ID_CLIENT_MESSAGE_RESPONSE: {
                struct response_data *resp = (struct response_data *)message->payload;
                struct client_context *context = (struct client_context *)self->context;
                if (context->req_type == ID_SERVER_MESSAGE_MUL_REQUEST) {
                        context->next_val = resp->sum;
                } else if (context->req_type == ID_SERVER_MESSAGE_SUM_REQUEST) {
                        context->prev_val = context->next_val;
                        context->next_val = resp->sum;
                }
                LOG_I("response for <%s>: %lld", self->desc->name, resp->sum);
                vPortFree(resp);
                break;
        }
        default:
                LOG_W("unsupported message id <%lu> for <%s>", message->id, self->desc->name);
                break;
        }
}

#define TASK_NAME                       "task"
#define TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)

static void task_service(void *pvParameters)
{
        LOG_I("started");

        while(1) {
                vTaskDelay(5000UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                size_t free_heap = xPortGetFreeHeapSize();
                LOG_T("tick = %lu, free_heap = %lu", current_tick, free_heap);
        }
}


int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_service, TASK_NAME, TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        g_node_server = node_create(&g_node_server_desc, NULL);
        configASSERT(g_node_server != NULL);
        g_node_client_1 = node_create(&g_node_client_1_desc, &g_node_client_1_context);
        configASSERT(g_node_client_1 != NULL);
        g_node_client_2 = node_create(&g_node_client_2_desc, &g_node_client_2_context);
        configASSERT(g_node_client_2 != NULL);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
