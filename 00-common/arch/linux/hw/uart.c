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
#include <stdio.h>
#include <string.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "system/log.h"

struct hw_uart {
        int file_desc;
        bool sending;
        SemaphoreHandle_t start_write;
        TaskHandle_t tx_task;
        TaskHandle_t rx_task;
};

#define RX_TASK_NAME               "rx_task"
#define RX_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define RX_TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE)

#define TX_TASK_NAME               "tx_task"
#define TX_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define TX_TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE)

#define TX_BUFFER_SIZE             16
#define RX_BUFFER_SIZE             16

static void rx_service(void *pvParameters)
{
        struct uart *self = (struct uart *)pvParameters;
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;

        uint8_t buf[RX_BUFFER_SIZE];

        while(1) {
                int rd = read(hw_uart->file_desc, buf, sizeof(buf));
                if (rd > 0) {
                        uart_read_callback(self, buf, rd, NULL);
                }
        }
}

static void tx_service(void *pvParameters)
{
        struct uart *self = (struct uart *)pvParameters;
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;

        uint8_t buf[TX_BUFFER_SIZE];

        while(1) {
                xSemaphoreTake(hw_uart->start_write, portMAX_DELAY);
                hw_uart->sending = true;

                size_t to_send = 0;
                do {
                        to_send = uart_write_callback(self, buf, sizeof(buf), NULL);
                        
                        int sent = 0;
                        while (sent < to_send) {
                                int s = write(hw_uart->file_desc, &buf[sent], to_send - sent);
                                if (s < 0) {
                                        if (errno != EAGAIN) {
                                                break;
                                        } else {
                                                s = 0;
                                        }
                                }
                                sent += s;
                        }
                } while (to_send > 0);

                hw_uart->sending = false;
        }
}

void hw_uart_start_write(struct uart *self)
{
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;
        xSemaphoreGive(hw_uart->start_write);
}

bool hw_uart_stop_write(struct uart *self)
{
        struct hw_uart *hw_uart = (struct hw_uart *)self->hw_driver;
        return hw_uart->sending;
}

static int convert_baudrate(uint32_t baudrate)
{
        switch(baudrate) {
        case 9600:
                return B9600;
        case 19200:
                return B19200;
        case 38400:
                return B38400;
        case 57600:
                return B57600;
        case 115200:
                return B115200;
        case 230400:
                return B230400;
        default:
                return -1;
        }
}
        
int hw_uart_init(struct uart *self)
{
        int ret;

        int br = convert_baudrate(self->baudrate);
        if (br < 0) {
                LOG_E("unsupported baudrate");
                return -1;
        }

        struct hw_uart *hw_uart = pvPortMalloc(sizeof(struct hw_uart));
        configASSERT(hw_uart != NULL);
        self->hw_driver = hw_uart;

        hw_uart->sending = false;

        hw_uart->file_desc = open(self->port, O_RDWR | O_NOCTTY);
        if (hw_uart->file_desc < 0) {
                LOG_E("cannot open port");
                vPortFree(hw_uart);
                return -1;
        }

        struct termios attr = {0};
        attr.c_cflag = CS8 | CLOCAL | CREAD;
        attr.c_iflag = IGNPAR;
        attr.c_oflag = 0;
        attr.c_lflag = 0;
        attr.c_cc[VMIN] = 0;
        attr.c_cc[VTIME] = 10;

        cfsetispeed(&attr, br);
        cfsetospeed(&attr, br);

        ret = tcsetattr(hw_uart->file_desc, TCSANOW, &attr);
        if(ret < 0) {
                LOG_E("cannot set port attributes");
                close(hw_uart->file_desc);
                flock(hw_uart->file_desc, LOCK_UN);
                vPortFree(hw_uart);
                return -1;
        }

        BaseType_t s;

        hw_uart->start_write = xSemaphoreCreateBinary();
        configASSERT(hw_uart->start_write != NULL);

        s = xTaskCreate(rx_service, RX_TASK_NAME, RX_TASK_STACK_SIZE, self, RX_TASK_PRIORITY, &hw_uart->rx_task);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(tx_service, TX_TASK_NAME, TX_TASK_STACK_SIZE, self, TX_TASK_PRIORITY, &hw_uart->tx_task);
        configASSERT(s != pdFALSE);

        return 0;
}
