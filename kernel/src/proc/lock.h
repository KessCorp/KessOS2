#ifndef LOCK_H
#define LOCK_H

// 2022 Ian Moffett <ian@kesscoin.com>


void mutex_lock(void* lock);
void mutex_unlock(void* lock);

#endif
