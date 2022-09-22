#ifndef _MEM_H_
#define _MEM_H_

// HEAP_SIZE limit 131072
#define HEAP_SIZE 0x0400

#include "stdint.h"

void *simple_malloc(uint32_t size);
void test_simple_malloc();

#endif