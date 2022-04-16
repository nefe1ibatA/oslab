#include "string.h"

void *memset(void *dst, int c, uint64 n) {
    char *cdst = (char *)dst;
    for (uint64 i = 0; i < n; ++i)
        cdst[i] = c;

    return dst;
}

void *memcpy(void *dst, void *src, uint64 n)
{
    char *tmp_src = (char *)src;
    char *tmp_dst = (char *)dst;
    for(uint64 i = 0; i < n; i++) {
        tmp_dst[i] = tmp_src[i];
    }
    return dst;
}