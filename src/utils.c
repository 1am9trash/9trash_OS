#include "utils.h"

uint32_t hex_to_int(char *hex, int len) {
    uint32_t val = 0;
    for (int i = 0; i < len && hex[i] != 0; i++) {
        val <<= 4;
        if (hex[i] >= 'A' && hex[i] <= 'F')
            val += hex[i] - 'A' + 10;
        else if (hex[i] >= 'a' && hex[i] <= 'f')
            val += hex[i] - 'a' + 10;
        else
            val += hex[i] - '0';
    }
    return val;
}

uint32_t convert_endian(uint32_t num) {
    uint32_t t[4];
    t[0] = (num & 0x000000ff) << 24;
    t[1] = (num & 0x0000ff00) << 8;
    t[2] = (num & 0x00ff0000) >> 8;
    t[3] = (num & 0xff000000) >> 24;
    return t[0] | t[1] | t[2] | t[3];
}

