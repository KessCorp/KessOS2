#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

// 2022 Ian Moffett <ian@kesscoin.com>

typedef enum {
   S_WARNING,
   S_INFO,
   S_CRITICAL,
   S_PANIC,
} STATUS;


void log(const char* format, STATUS status, ...);
void dump_stack(void* rsp);

#endif
