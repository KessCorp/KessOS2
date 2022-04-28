#include <proc/lock.h>

// 2022 Ian Moffett <ian@kesscoin.com>

void mutex_lock(void* lock) {
  while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE));
}


void mutex_unlock(void* lock) {
  __atomic_clear(lock, __ATOMIC_RELEASE);
}
