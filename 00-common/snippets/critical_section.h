#pragma once

#include "FreeRTOS.h"

class OSCriticalSection {
public:
    OSCriticalSection()
    {
        portENTER_CRITICAL();
    };

    ~OSCriticalSection()
    {
        portEXIT_CRITICAL();
    };
};

/*

void super_important_code(void)
{
    prepare_for_operation();

    {
        OSCriticalSection section;

        do_some_very_important_work();
    }

    process_results();
}

*/