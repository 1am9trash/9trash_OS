#ifndef _STRING_H_
#define _STRING_H_

#include "stdint.h"

int strcmp(char *s1, char *s2);
void strcpy(char *src, char *dst);
void strncpy(char *src, char *dst, uint32_t size);
uint32_t strlen(char *src);

#endif