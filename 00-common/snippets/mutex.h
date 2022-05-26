#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

class SharedResource {
    SemaphoreHandle_t m_mutex;

public:
    SharedResource()
    {
        this->m_mutex = xSemaphoreCreateMutex();
    }

    SemaphoreHandle_t mutex()
    {
        return this->m_mutex;
    }
};

class OSMutex {
    SharedResource &m_resource;

public:
    OSMutex(SharedResource resource): m_resource(resource)
    {
        xSemaphoreTake(this->m_resource.mutex(), portMAX_DELAY);
    }

    ~OSMutex(void)
    {
        xSemaphoreGive(this->m_resource.mutex());
    }
};

/*

static SharedResource g_resource;

void super_important_code(void)
{
    prepare_for_operation();

    {
        OSMutex mutex(g_resource);

        safe_access_to_shared_resource();
    }

    process_results();
}

*/