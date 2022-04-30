#include <proc/thread.h>
#include <util/etc.h>
#include <util/asm.h>

#define MAX_THREADS 3


SECTION_DATA static uint64_t threads[3];
SECTION_DATA static uint8_t ctid = 0;
SECTION_DATA static uint8_t nthreads = 0;


void mkthread(uint64_t exec_addr) {
    UNGLITCH_ARG(exec_addr);
    threads[nthreads++] = exec_addr;
}


void unlist_thread(int tid) {
    if (tid > MAX_THREADS - 1) return;
    threads[tid] = 0x0;
}


uint8_t get_active_thread() {
    return ctid;
}


void thread_pit_schedule() {
    if (ctid >= MAX_THREADS - 1) return;
    if (threads[ctid] == 0) return;

    void(*exec)(void) = (void*)threads[ctid++];
    exec();
}
