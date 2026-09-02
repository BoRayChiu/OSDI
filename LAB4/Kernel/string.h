#ifndef STRING_H
#define STRING_H

int strcmp(const char *s1, const char *s2);
char* itoa(const unsigned long value, const int base);
void memzero(void *dst, unsigned long size);

#endif