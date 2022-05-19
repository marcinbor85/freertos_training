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

#include "hal/uart_driver.h"

#include <stdbool.h>
#include <string.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "FreeRTOS.h"

extern uint32_t SystemCoreClock;

struct hw_uart {
        uint32_t uart;
        bool sending;
};

static struct uart *g_uart_array[4];

int hw_uart_init(struct uart *self)
{
        struct hw_uart *hw_uart = pvPortMalloc(sizeof(struct hw_uart));
        configASSERT(hw_uart != NULL);

        self->hw_driver = hw_uart;
        hw_uart->sending = false;

        if (strcmp(self->port, "uart3") == 0) {
                hw_uart->uart = USART3;
                g_uart_array[3] = self;

                rcc_periph_clock_enable(RCC_GPIOC);
                rcc_periph_clock_enable(RCC_USART3);

                gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11);
                gpio_set_af(GPIOC, GPIO_AF7, GPIO10);
                gpio_set_af(GPIOC, GPIO_AF7, GPIO11);

                usart_set_baudrate(USART3, self->baudrate);
                usart_set_databits(USART3, 8);
                usart_set_stopbits(USART3, USART_STOPBITS_1);
                usart_set_mode(USART3, USART_MODE_TX_RX);
                usart_set_parity(USART3, USART_PARITY_NONE);
                usart_enable_rx_interrupt(USART3);
                usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

                nvic_set_priority(NVIC_USART3_IRQ, 0xFF);
                nvic_enable_irq(NVIC_USART3_IRQ);

	        usart_enable(USART3);
        } else {
                vPortFree(hw_uart);
                return -1;
        }

        return 0;
}

void hw_uart_start_write(struct uart *self)
{
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;
        hw_uart->sending = true;
        USART_CR1(hw_uart->uart) |= USART_CR1_TXEIE;
}

bool hw_uart_stop_write(struct uart *self)
{
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;
        USART_CR1(hw_uart->uart) &= ~USART_CR1_TXEIE;
        return hw_uart->sending;
}

void USART3_IRQHandler(void)
{
        struct uart *self = g_uart_array[3];
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;

        BaseType_t need_switch = 0;
        uint8_t b;

        if ((USART_ISR(hw_uart->uart) & USART_FLAG_ORE) != 0)
                USART_ICR(hw_uart->uart) = USART_FLAG_ORE;

        if ((USART_CR1(hw_uart->uart) & USART_CR1_TXEIE) != 0) {
                if ((USART_ISR(hw_uart->uart) & USART_ISR_TXE) != 0) {
                        size_t to_write = uart_write_callback(self, &b, 1, &need_switch);
                        if (to_write == 0) {
                                USART_CR1(hw_uart->uart) &= ~USART_CR1_TXEIE;
                                hw_uart->sending = false;
                        } else {
                                USART_TDR(hw_uart->uart) = b;
                        }
                }
        }

        while ((USART_ISR(hw_uart->uart) & USART_ISR_RXNE) != 0) {
                b = USART_RDR(hw_uart->uart);
                (void)uart_read_callback(self, &b, 1, &need_switch);
        }

        portYIELD_FROM_ISR(need_switch);
}
