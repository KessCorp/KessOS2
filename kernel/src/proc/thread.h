#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

void mkthread(uint64_t exec_addr);
void thread_pit_schedule();             // DO NOT USE.
void unlist_thread(int tid);
uint8_t get_active_thread();

#endif
