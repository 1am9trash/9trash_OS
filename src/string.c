#include "string.h"

int strcmp(char *s1, char *s2) {
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*(uint8_t *)s1) - (*(uint8_t *)s2);
}

void strcpy(char *src, char *dst) {
    while (*src != '\0') {
        *dst = *src;
        src++;
        dst++;
    }
    dst = '\0';
}

void strncpy(char *src, char *dst, uint32_t size) {
    for (int i = 0; i < size; i++) {
        dst[i] = src[i];
        if (src[i] == '\0') 
            break;
    }
}

uint32_t strlen(char *src) {
    uint32_t len = 0;
    while (*src != '\0') {
        src++;
        len++;
    }
    return len;
}