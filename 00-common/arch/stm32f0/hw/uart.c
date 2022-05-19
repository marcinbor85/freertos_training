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

#include <stm32f0xx.h>

#include "FreeRTOS.h"

extern uint32_t SystemPeripheralClock;

struct hw_uart {
        USART_TypeDef *uart;
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

                RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
                RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

                // USART3 - GPIOC10 - tx
                GPIOC->MODER &= ~GPIO_MODER_MODER10;
                GPIOC->MODER |= GPIO_MODER_MODER10_1;
                GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL10;
                GPIOC->AFR[1] |= (0x01 << GPIO_AFRH_AFSEL10_Pos);

                // USART3 - GPIOC11 - rx
                GPIOC->MODER &= ~GPIO_MODER_MODER11;
                GPIOC->MODER |= GPIO_MODER_MODER11_1;
                GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL11;
                GPIOC->AFR[1] |= (0x01 << GPIO_AFRH_AFSEL11_Pos);

                // USART3 - config and enable
                hw_uart->uart->CR1 = 0;
                hw_uart->uart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
                hw_uart->uart->BRR = SystemPeripheralClock / self->baudrate;
                hw_uart->uart->CR1 |= USART_CR1_UE;

                NVIC_EnableIRQ(USART3_4_IRQn);
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
        hw_uart->uart->CR1 |= USART_CR1_TXEIE;
}

bool hw_uart_stop_write(struct uart *self)
{
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;
        hw_uart->uart->CR1 &= ~USART_CR1_TXEIE;
        return hw_uart->sending;
}

void USART3_4_IRQHandler(void)
{
        struct uart *self = g_uart_array[3];
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;

        BaseType_t need_switch = 0;
        uint8_t b;

        if ((hw_uart->uart->ISR & USART_ISR_ORE) != 0)
                hw_uart->uart->ICR = USART_ISR_ORE;

        if ((hw_uart->uart->CR1 & USART_CR1_TXEIE) != 0) {
                if ((hw_uart->uart->ISR & USART_ISR_TXE) != 0) {
                        size_t to_write = uart_write_callback(self, &b, 1, &need_switch);
                        if (to_write == 0) {
                                hw_uart->uart->CR1 &= ~USART_CR1_TXEIE;
                                hw_uart->sending = false;
                        } else {
                                hw_uart->uart->TDR = b;
                        }
                }
        }

        while ((hw_uart->uart->ISR & USART_ISR_RXNE) != 0) {
                b = hw_uart->uart->RDR;
                (void)uart_read_callback(self, &b, 1, &need_switch);
        }

        portYIELD_FROM_ISR(need_switch);
}
