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

#ifndef _LOG_H
#define _LOG_H

#define LOGGING_LEVEL_NONE      0
#define LOGGING_LEVEL_ERROR     10
#define LOGGING_LEVEL_WARNING   20
#define LOGGING_LEVEL_INFO      30
#define LOGGING_LEVEL_DEBUG     40

#ifndef LOGGING_ENABLED
#define LOGGING_ENABLED 1
#endif

#ifndef LOGGING_LEVEL
#define LOGGING_LEVEL LOGGING_LEVEL_DEBUG
#endif

#ifdef LOGGING_ENABLED

#if LOGGING_LEVEL >= LOGGING_LEVEL_ERROR
#define LOG_E(fmt, ...) { log_printf(LOGGING_LEVEL_ERROR, __FILE__, __func__, __LINE__, fmt, __VA_ARGS__); }
#else
#define LOG_E(fmt, ...)
#endif

#if LOGGING_LEVEL >= LOGGING_LEVEL_WARNING
#define LOG_W(fmt, ...) { log_printf(LOGGING_LEVEL_WARNING, __FILE__, __func__, __LINE__, fmt, __VA_ARGS__); }
#else
#define LOG_W(fmt, ...)
#endif

#if LOGGING_LEVEL >= LOGGING_LEVEL_INFO
#define LOG_I(fmt, ...) { log_printf(LOGGING_LEVEL_INFO, __FILE__, __func__, __LINE__, fmt, __VA_ARGS__); }
#else
#define LOG_I(fmt, ...)
#endif

#if LOGGING_LEVEL >= LOGGING_LEVEL_DEBUG
#define LOG_D(fmt, ...) { log_printf(LOGGING_LEVEL_DEBUG, __FILE__, __func__, __LINE__, fmt, __VA_ARGS__); }
#else
#define LOG_D(fmt, ...)
#endif

void log_printf(int level, const char *file, const char *func, unsigned long line, const char *fmt, ...);

#else

#define LOG_D(fmt, ...)
#define LOG_I(fmt, ...)
#define LOG_W(fmt, ...)
#define LOG_E(fmt, ...)

#endif

#endif /* _LOG_H */
