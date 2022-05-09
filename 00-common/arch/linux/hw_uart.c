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

#include "uart_impl.h"

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

#include "log.h"

static int g_file_desc;
static struct termios g_port_configs;
static struct termios g_old_port_config;

#define PORT_NAME       "/dev/ttyUSB0"

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
        uint8_t buf[RX_BUFFER_SIZE];

        while(1) {
                int rd = read(g_file_desc, buf, sizeof(buf));
                if (rd > 0) {
                        uart_read_callback(buf, rd, NULL);
                }
        }
}

static void tx_service(void *pvParameters)
{
        uint8_t buf[TX_BUFFER_SIZE];

        while(1) {
                size_t to_send = uart_write_callback(buf, sizeof(buf), NULL);
                
                int sent = 0;
                while (sent < to_send) {
                        int s = write(g_file_desc, &buf[sent], to_send - sent);
                        if (s < 0) {
                                if (errno != EAGAIN) {
                                        break;
                                } else {
                                        s = 0;
                                }
                        }
                        sent += s;
                }
        }
}

void hw_uart_start_write(void)
{

}

bool hw_uart_stop_write(void)
{
        return false;
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
        
int hw_uart_init(uint32_t baudrate)
{
        int ret;

        int br = convert_baudrate(baudrate);
        if (br < 0) {
                LOG_E("unsupported baudrate");
                return -1;
        }

        g_file_desc = open(PORT_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
        if (g_file_desc < 0) {
                LOG_E("cannot open port");
                return -1;
        }

        ret = flock(g_file_desc, LOCK_EX | LOCK_NB);
        if (ret != 0) {
                LOG_E("cannot lock port");
                close(g_file_desc);
                return -1;
        }

        ret = tcgetattr(g_file_desc, &g_old_port_config);
        if (ret < 0) {
                LOG_E("cannot get port attributes");
                close(g_file_desc);
                flock(g_file_desc, LOCK_UN);
                return -1;
        }

        memset((uint8_t*)&g_port_configs, 0, sizeof(g_port_configs));

        g_port_configs.c_cflag = CS8 | CLOCAL | CREAD;
        g_port_configs.c_iflag = IGNPAR;
        g_port_configs.c_oflag = 0;
        g_port_configs.c_lflag = 0;
        g_port_configs.c_cc[VMIN] = 0;
        g_port_configs.c_cc[VTIME] = 0;

        cfsetispeed(&g_port_configs, br);
        cfsetospeed(&g_port_configs, br);

        ret = tcsetattr(g_file_desc, TCSANOW, &g_port_configs);
        if(ret < 0) {
                LOG_E("cannot set port attributes");
                tcsetattr(g_file_desc, TCSANOW, &g_old_port_config);
                close(g_file_desc);
                flock(g_file_desc, LOCK_UN);
                return -1;
        }

        BaseType_t s;

        s = xTaskCreate(rx_service, RX_TASK_NAME, RX_TASK_STACK_SIZE, NULL, RX_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(tx_service, TX_TASK_NAME, TX_TASK_STACK_SIZE, NULL, TX_TASK_PRIORITY, NULL);
        configASSERT(s != pdFALSE);

        return 0;
}

