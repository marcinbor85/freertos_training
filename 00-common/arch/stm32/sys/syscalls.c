/*
MIT License

Copyright (c) 2020 Marcin Borowicz

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

#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <stm32f0xx.h>

extern int errno;

void *_sbrk(int incr) {

    extern const void *_heap;
    extern const void *_eheap;

    void *prev_heap_ptr;
    static void *heap_ptr;

    if (heap_ptr == 0) {
        heap_ptr = (void *)&_heap;
    }

    void * next_heap_ptr = (void*)((int)heap_ptr + incr);

    if (next_heap_ptr >= (void *) &_eheap) {
        errno = ENOMEM;
        return NULL;
    }

    prev_heap_ptr = heap_ptr;
    heap_ptr = next_heap_ptr;

    return (void *)prev_heap_ptr;
}

int _close(int file)
{
        return -1;
}

int _fstat(int file, struct stat *st)
{
        st->st_mode = S_IFCHR;

        return 0;
}

int _isatty(int file)
{
        return 1;
}

int _lseek(int file, int ptr, int dir)
{
        return 0;
}

void _exit(int status)
{
        __asm("BKPT #0");

        while(1) {};
}

void _kill(int pid, int sig)
{
        return;
}

int _getpid(void)
{
        return -1;
}

int _write(int file, char *ptr, int len)
{
        for (int i = 0; i < len; i++) {
                while ((USART2->ISR & USART_ISR_TC) == 0) {};
                USART2->ICR |= USART_ICR_TCCF;
                USART2->TDR = *ptr++;
        }
        return len;
}

int _read(int file, char *ptr, int len)
{
        return 0;
}
