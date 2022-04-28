#include <debug/log.h>
#include <util/string.h>
#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>


void dump_stack(void* rsp) {	
	log("-----------------------------\n", -1);
	log("ADDRESS         DATA\n", -1);
		
	for (int i = 0; i < 5; ++i) {
		log("%x => %x\n", -1, (uint64_t)rsp, *(char*)rsp);
		++rsp;
	}

	log("-----------------------------\n", -1);
}
