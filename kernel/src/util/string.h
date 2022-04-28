#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

extern char res[25];

size_t strlen(const char* const STR);
char* dec2str(int dec);
uint8_t* hex2str(uint32_t hex_num);
uint8_t* strncpy(uint8_t *dst, const uint8_t *src, const uint8_t len);
uint8_t strncmp(char* s1, char* s2, uint32_t n);
#endif
