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

#include <stm32f0xx.h>

#include "FreeRTOS.h"

extern uint32_t SystemPeripheralClock;

static bool g_sending;

int hw_uart_init(uint32_t baudrate)
{
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
        USART3->CR1 = 0;
        USART3->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
        USART3->BRR = SystemPeripheralClock / baudrate;
        USART3->CR1 |= USART_CR1_UE;

        NVIC_EnableIRQ(USART3_4_IRQn);

        return 0;
}

void hw_uart_start_write(void)
{
        g_sending = true;
        USART3->CR1 |= USART_CR1_TXEIE;
}

bool hw_uart_stop_write(void)
{
        USART3->CR1 &= ~USART_CR1_TXEIE;
        return g_sending;
}

void USART3_4_IRQHandler(void)
{
        BaseType_t needSwitch = 0;
        uint8_t b;

        if ((USART3->ISR & USART_ISR_ORE) != 0)
                USART3->ICR = USART_ISR_ORE;

        if ((USART3->CR1 & USART_CR1_TXEIE) != 0) {
                if ((USART3->ISR & USART_ISR_TXE) != 0) {
                        size_t to_write = uart_write_callback(&b, 1, &needSwitch);
                        if (to_write == 0) {
                                USART3->CR1 &= ~USART_CR1_TXEIE;
                                g_sending = false;
                        } else {
                                USART3->TDR = b;
                        }
                }
        }

        while ((USART3->ISR & USART_ISR_RXNE) != 0) {
                b = USART3->RDR;
                (void)uart_read_callback(&b, 1, &needSwitch);
        }

        portYIELD_FROM_ISR(needSwitch);
}
