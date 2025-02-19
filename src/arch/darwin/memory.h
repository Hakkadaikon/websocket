#ifndef NOSTR_DARWIN_MEMORY_H_
#define NOSTR_DARWIN_MEMORY_H_

#include <string.h>

#include "../../util/types.h"

static inline void* darwin_memcpy(void* dest, const void* src, size_t size)
{
    return memcpy(dest, src, size);
}

static inline void* darwin_memset(void* s, const int32_t c, const size_t size)
{
    return memset(s, c, size);
}

/*
 * memset_s: Secure memory set function (wrapper for memset)
 *
 * @s: Pointer to the memory area to be set
 * @smax: Size of the memory area in bytes
 * @c: Value to set (passed as int, but treated as an unsigned char)
 * @n: Number of bytes to set
 *
 * Note: A compiler barrier is inserted to ensure that the memset call is not optimized away.
 */
static inline int darwin_memset_s(void* s, const size_t smax, const int c, const size_t n)
{
    return memset_s(s, smax, c, n);
}

#endif
