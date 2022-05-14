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

#include "led.h"

#include "system/log.h"

#if defined(BOARD_NUCLEO_STM32F0)

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_RCC         RCC_GPIOA
#define LED_PORT        GPIOA
#define LED_PIN         GPIO5

int bsp_led_init(void)
{
        rcc_periph_clock_enable(LED_RCC);
        gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);

        return 0;
}

void bsp_led_set_state(bool state)
{
        void (*setter)(uint32_t, uint16_t) = (state != false) ? gpio_set : gpio_clear;
        setter(LED_PORT, LED_PIN);
}

#elif defined(BOARD_PC_LINUX)

int bsp_led_init(void)
{
        LOG_W("led not supported, use emulation instead");
        return 0;
}

void bsp_led_set_state(bool state)
{
        LOG_W("led state = %d", state);
}

#else
        #error "Unsupported board"
#endif

