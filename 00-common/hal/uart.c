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

#include "uart.h"
#include "uart_driver.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "system/log.h"

size_t uart_write_callback(struct uart *self, uint8_t *data, size_t size, BaseType_t *token)
{
        size_t s;
        s = xStreamBufferReceiveFromISR(self->tx_stream, data, size, token);
        return s;
}

size_t uart_read_callback(struct uart *self, uint8_t *data, size_t size, BaseType_t *token)
{
        size_t s;
        s = xStreamBufferSendFromISR(self->rx_stream, data, size, token);
        return s;
}

size_t uart_write(struct uart *self, uint8_t *data, size_t size, TickType_t timeout)
{
        size_t sent = 0;
        if (size == 0)
                return 0;

        bool sending = hw_uart_stop_write(self);
        sent = xStreamBufferSend(self->tx_stream, data, size, timeout);
        if (sent > 0) {
                LOG_D("sent %d bytes", sent);
                hw_uart_start_write(self);
        } else {
                if (sending != false)
                        hw_uart_start_write(self);
        }
        return sent;
}

size_t uart_read(struct uart *self, uint8_t *data, size_t size, TickType_t timeout)
{
        size_t recv = 0;
        recv = xStreamBufferReceive(self->rx_stream, data, size, timeout);
        if (recv > 0) {
                LOG_D("received %d bytes", recv);
        }
        return recv;
}

struct uart* uart_open(const char *port, uint32_t baudrate, size_t tx_buf_size, size_t rx_buf_size)
{        
        struct uart *self = pvPortMalloc(sizeof(struct uart));
        configASSERT(self != NULL);

        self->baudrate = baudrate;
        self->port = port;
        self->hw_driver = NULL;
        
        self->tx_stream = xStreamBufferCreate(tx_buf_size, 0);
        configASSERT(self->tx_stream != pdFALSE);

        self->rx_stream = xStreamBufferCreate(rx_buf_size, 0);
        configASSERT(self->rx_stream != pdFALSE);

        int s = hw_uart_init(self);
        if (s != 0) {
                LOG_E("cannot open port: %s", self->port);
                vPortFree(self);
                self = NULL;
        } else {
                LOG_I("initialized");
        }

        return self;
}
