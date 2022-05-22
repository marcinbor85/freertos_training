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

#ifndef _TRACE_H
#define _TRACE_H

#include <stdio.h>
#include <stdint.h>

#define traceQUEUE_CREATE(x)                    { printf("traceQUEUE_CREATE: 0x%08lX\r\n", (uint32_t)x); }
#define traceQUEUE_SEND(x)                      { printf("traceQUEUE_SEND: 0x%08lX\r\n", (uint32_t)x); }
#define traceQUEUE_SEND_FROM_ISR(x)             { printf("traceQUEUE_SEND_FROM_ISR: 0x%08lX\r\n", (uint32_t)x); }
#define traceQUEUE_RECEIVE(x)                   { printf("traceQUEUE_RECEIVE: 0x%08lX\r\n", (uint32_t)x); }

// #define traceTASK_SWITCHED_IN()              { printf("traceTASK_SWITCHED_IN: <%s>\r\n", pxCurrentTCB->pcTaskName); }
// #define traceTASK_SWITCHED_OUT()             { printf("traceTASK_SWITCHED_OUT: <%s>\r\n", pxCurrentTCB->pcTaskName); }

#define traceMALLOC( pvAddress, uiSize )        { printf("traceMALLOC: 0x%08lX [%d]\r\n", (uint32_t)pvAddress, uiSize); }
#define traceFREE( pvAddress, uiSize )          { printf("traceFREE: 0x%08lX [%d]\r\n", (uint32_t)pvAddress, uiSize); }

#endif /* _TRACE_H */
