#ifndef _UTILS_H_
#define _UTILS_H_

#include "stdint.h"

uint32_t hex_to_int(char *hex, int len);
uint32_t convert_endian(uint32_t num);

#endif