#include "worker.h"

#include <stdbool.h>

typedef enum {
        WORKER_CMD_CALL_AFTER,
        WORKER_CMD_CANCEL,
} worker_cmd_t;

struct worker_cmd {
        worker_cmd_t type;
        struct worker_job *job;
};

static TickType_t get_remaining_time(struct worker_job *job, TickType_t now)
{
        TickType_t ret;

        if (now - job->start_time >= job->delay) {
                ret = 0;
        } else {
                ret =  job->delay + job->start_time - now;
        }

        return ret;
}

static void add_worker_job(struct worker_manager *self, struct worker_job *job)
{
        struct worker_job *next_job = self->jobs;
        struct worker_job *prev_job = NULL;

        TickType_t now = xTaskGetTickCount();

        if (next_job == NULL) {
                // insert at the beginning
                job->next = NULL;
                self->jobs = job;
                return;
        }

        TickType_t remaining_time = get_remaining_time(job, now);

        while (next_job != NULL) {
                TickType_t next_job_remaining_time = get_remaining_time(next_job, now);

                if (remaining_time < next_job_remaining_time) {
                        // insert before next job
                        job->next = next_job;
                        if (prev_job == NULL) {
                                self->jobs = job;
                        } else {
                                prev_job->next = job;
                        }
                        break;
                } else {
                        if (next_job->next == NULL) {
                                // insert after current job
                                job->next = next_job->next;
                                next_job->next = job;
                                break;
                        }
                }

                prev_job = next_job;
                next_job = next_job->next;
        }
}

static void remove_worker_job(struct worker_manager *self, struct worker_job *job)
{
        struct worker_job *next_job = self->jobs;
        struct worker_job *prev_job = NULL;

        while (next_job != NULL) {
                if (job == next_job) {
                        if (prev_job == NULL) {
                                self->jobs = next_job->next;
                        } else {
                                prev_job->next = next_job->next;
                        }
                        vPortFree(next_job);
                        break;
                }

                prev_job = next_job;
                next_job = next_job->next;
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
                struct worker_job *current_job = self->jobs;

                timeout = (current_job == NULL) ? portMAX_DELAY : get_remaining_time(current_job, now);

                s = xQueueReceive(self->queue, &cmd, timeout);
                if (s == pdFALSE) {
                        current_job->callback(self, current_job);
                        self->jobs = current_job->next;
                        vPortFree(current_job);
                } else {
                        switch (cmd.type) {
                        case WORKER_CMD_CALL_AFTER:
                                add_worker_job(self, cmd.job);
                                break;
                        case WORKER_CMD_CANCEL:
                                remove_worker_job(self, cmd.job);
                                break;
                        default:
                                configASSERT(false);
                                break;
                        }
                }
        }
}

struct worker_manager* worker_create(const char *name, uint32_t stack_size, UBaseType_t priority, UBaseType_t cmd_queue_size, void *context)
{
        struct worker_manager *self = pvPortMalloc(sizeof(struct worker_manager));
        configASSERT(self != NULL);

        self->jobs = NULL;
        self->context = context;

        BaseType_t s;
        
        s = xTaskCreate(worker_task, name, stack_size, self, priority, &self->task);     
        configASSERT(s != pdFALSE);

        self->queue = xQueueCreate(cmd_queue_size, sizeof(struct worker_cmd));
        configASSERT(self->queue != NULL);

        return self;
}

struct worker_job* worker_call_after(struct worker_manager *self, TickType_t delay, worker_callback_t callback, void *context)
{
        struct worker_job *job = pvPortMalloc(sizeof(struct worker_job));
        configASSERT(job != NULL);

        if (job == NULL)
                return NULL;

        job->next = NULL;
        job->callback = callback;
        job->context = context;
        job->delay = delay;
        job->start_time = xTaskGetTickCount();

        struct worker_cmd cmd = {
                .type = WORKER_CMD_CALL_AFTER,
                .job = job,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);

        return job;
}

void worker_cancel(struct worker_manager *self, struct worker_job *job)
{
        struct worker_cmd cmd = {
                .type = WORKER_CMD_CANCEL,
                .job = job,
        };
        BaseType_t s = xQueueSend(self->queue, &cmd, portMAX_DELAY);
        configASSERT(s != pdFALSE);
}
