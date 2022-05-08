#ifndef _MACRO_H
#define _MACRO_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define CREATE_MODULE_MUTEX \
        static xSemaphoreHandle g_mutex;\
        static void mutex_lazy_init(void) \
        { \
                if (g_mutex != NULL) \
                        return; \
                g_mutex = xSemaphoreCreateMutex(); \
        } \
        static void lock(void) \
        { \
                mutex_lazy_init(); \
                xSemaphoreTake(g_mutex, portMAX_DELAY); \
        } \
        static void unlock(void) \
        { \
                mutex_lazy_init(); \
                xSemaphoreGive(g_mutex); \
        } \

#define LOCK()          lock()
#define UNLOCK()        unlock()

#endif /* _MACRO_H */
