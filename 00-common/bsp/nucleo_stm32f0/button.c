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

#include "bsp/button.h"

#include "system/log.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

#define BUTTON_RCC              RCC_GPIOC
#define BUTTON_PORT             GPIOC
#define BUTTON_PIN              GPIO13
#define BUTTON_EXTI             EXTI13

static enum exti_trigger_type g_button_trigger_type;

static QueueHandle_t g_button_queue;

#define BUTTON_QUEUE_SIZE      2

static void set_trigger_type(enum exti_trigger_type type)
{
        g_button_trigger_type = type;
        exti_set_trigger(BUTTON_EXTI, type);
}

int bsp_button_init(void)
{
        rcc_periph_clock_enable(RCC_SYSCFG_COMP);
        rcc_periph_clock_enable(BUTTON_RCC);
        gpio_mode_setup(BUTTON_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BUTTON_PIN);

        g_button_queue = xQueueCreate(BUTTON_QUEUE_SIZE, sizeof(bsp_button_event_t));
        configASSERT(g_button_queue != NULL);

        nvic_enable_irq(NVIC_EXTI4_15_IRQ);

        set_trigger_type(EXTI_TRIGGER_FALLING);
        exti_select_source(BUTTON_EXTI, BUTTON_PORT);
        exti_enable_request(BUTTON_EXTI);

        return 0;
}

void EXTI4_15_IRQHandler(void)
{
        BaseType_t need_switch = 0;
        bsp_button_event_t event;

        exti_reset_request(BUTTON_EXTI);

        if (g_button_trigger_type == EXTI_TRIGGER_FALLING) {
                set_trigger_type(EXTI_TRIGGER_RISING);
                event = BSP_BUTTON_EVENT_PRESSED;
        } else {
                set_trigger_type(EXTI_TRIGGER_FALLING);
                event = BSP_BUTTON_EVENT_RELEASED;
        }

        xQueueSendFromISR(g_button_queue, &event, &need_switch);

        portYIELD_FROM_ISR(need_switch);
}

bool bsp_button_is_pressed(void)
{
        uint16_t s = gpio_get(BUTTON_PORT, BUTTON_PIN);
        return (s == 0) ? true : false;
}

bsp_button_event_t bsp_button_wait(TickType_t timeout)
{
        bsp_button_event_t event = BSP_BUTTON_EVENT_TIMEOUT;
        xQueueReceive(g_button_queue, &event, timeout);
        return event;
}
