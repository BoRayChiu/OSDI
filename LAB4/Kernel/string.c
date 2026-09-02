#include "string.h"

int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return (*(unsigned char*)s1 - *(unsigned char*)s2);
}

char* itoa(const unsigned long value, const int base) {
    static char buffer[33];
    char *ptr = &buffer[sizeof(buffer) - 1];
    *ptr = '\0';

    if (value == 0) {
        *--ptr = '0';
        return ptr;
    }

    unsigned long num = value;
    while (num > 0) {
        int digit = num % base;
        *--ptr = digit < 10 ? '0' + digit : 'A' + digit - 10;
        num /= base;
    }

    return ptr;
}

void memzero(void *dst, unsigned long size) {
    unsigned char *p = (unsigned char *)dst;
    while (size--) {
        *p++ = 0;
    }
}