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

#include "bsp/uart.h"

#include "hal/uart.h"

#define UART_BAUDRATE           115200
#define UART_PORT               "/dev/ttyUSB0"
#define UART_TX_BUF_SIZE        64
#define UART_RX_BUF_SIZE        64

static struct uart *g_uart;

int bsp_uart_init(void)
{
        g_uart = uart_open(UART_PORT, UART_BAUDRATE, UART_TX_BUF_SIZE, UART_RX_BUF_SIZE);
        configASSERT(g_uart != NULL);
        return (g_uart != NULL) ? 0 : -1;
}

size_t bsp_uart_write(uint8_t *data, size_t size, TickType_t timeout)
{
        return uart_write(g_uart, data, size, timeout);
}

size_t bsp_uart_read(uint8_t *data, size_t size, TickType_t timeout)
{
        return uart_read(g_uart, data, size, timeout);
}
