#include "string.h"

int strcmp(char *s1, char *s2) {
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*(uint8_t *)s1) - (*(uint8_t *)s2);
}