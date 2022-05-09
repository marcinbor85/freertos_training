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
#include "uart_impl.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "log.h"

#define TX_STREAM_BUFFER_SIZE      64
#define RX_STREAM_BUFFER_SIZE      64

static StreamBufferHandle_t g_rx_stream;
static StreamBufferHandle_t g_tx_stream;

size_t uart_write_callback(uint8_t *data, size_t size, BaseType_t *token)
{
        size_t s;
        s = xStreamBufferReceiveFromISR(g_tx_stream, data, size, token);
        return s;
}

size_t uart_read_callback(uint8_t *data, size_t size, BaseType_t *token)
{
        size_t s;
        s = xStreamBufferSendFromISR(g_rx_stream, data, size, token);
        return s;
}

size_t uart_write(uint8_t *data, size_t size, TickType_t timeout)
{
        size_t sent = 0;
        if (size == 0)
                return 0;

        bool sending = hw_uart_stop_write();
        sent = xStreamBufferSend(g_tx_stream, data, size, timeout);
        if (sent > 0) {
                LOG_D("sent %d bytes", sent);
                hw_uart_start_write();
        } else {
                if (sending != false)
                        hw_uart_start_write();
        }
        return sent;
}

size_t uart_read(uint8_t *data, size_t size, TickType_t timeout)
{
        size_t recv = 0;
        recv = xStreamBufferReceive(g_rx_stream, data, size, timeout);
        if (recv > 0) {
                LOG_D("received %d bytes", recv);
        }
        return recv;
}

int uart_init(uint32_t baudrate)
{        
        g_tx_stream = xStreamBufferCreate(TX_STREAM_BUFFER_SIZE, 0);
        configASSERT(g_tx_stream != pdFALSE);

        g_rx_stream = xStreamBufferCreate(RX_STREAM_BUFFER_SIZE, 0);
        configASSERT(g_rx_stream != pdFALSE);

        int s = hw_uart_init(baudrate);
        if (s != 0) {
                LOG_E("init error: %d", s);
        } else {
                LOG_I("initialized");
        }

        return s;
}
