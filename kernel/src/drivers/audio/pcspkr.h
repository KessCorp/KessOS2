#ifndef PCSPKR_H
#define PCSPKR_H

#include <util/etc.h>
#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>


void pcspkr_play(uint32_t hz);
void pcspkr_shutup();


#endif
