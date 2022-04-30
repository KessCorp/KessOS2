#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

void mkthread(uint64_t exec_addr);
void thread_pit_schedule();             // DO NOT USE.

#endif
