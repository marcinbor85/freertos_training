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

#ifndef _HAL_UART_H
#define _HAL_UART_H

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"

struct uart {
        char const* port;
        uint32_t baudrate;

        StreamBufferHandle_t tx_stream;
        StreamBufferHandle_t rx_stream;

        void *hw_driver;
};

struct uart* uart_open(const char *port, uint32_t baudrate, size_t tx_buf_size, size_t rx_buf_size);

size_t uart_write(struct uart *self, uint8_t *data, size_t size, TickType_t timeout);
size_t uart_read(struct uart *self, uint8_t *data, size_t size, TickType_t timeout);

#endif /* _HAL_UART_H */
