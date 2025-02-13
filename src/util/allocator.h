#ifndef NOSTR_ALLOCATOR_H_
#define NOSTR_ALLOCATOR_H_

#include <alloca.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <string.h>  // for memset
#endif

#define ALLOCATE_STACK
// #define ALLOCATE_HEAP

#if defined(ALLOCATE_STACK)
#define websocket_alloc(size) alloca(size)
#define websocket_free(addr)
#undef ALLOCATE_STACK
#elif defined(ALLOCATE_HEAP)
#define websocket_alloc(size) malloc(size)
#define websocket_free(addr) free(addr)
#undef ALLOCATE_HEAP
#endif

typedef size_t rsize_t;
typedef int    errno_t;

#ifdef __APPLE__
#define websocket_memset memset
#else
static inline errno_t websocket_memset(void* s, int c, rsize_t n)
{
    if (s == NULL && n != 0) {
        return EINVAL;
    }

    unsigned char* p = (unsigned char*)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }

    return 0;
}
#endif

#ifndef __APPLE__
#ifndef __STDC_LIB_EXT1__
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
static inline errno_t websocket_memset_s(void* s, rsize_t smax, int c, rsize_t n)
{
    if (s == NULL && n != 0) {
        return EINVAL;
    }

    // If n is greater than smax, clear the entire buffer (if possible) and return an error
    if (n > smax) {
        if (s != NULL && smax > 0) {
            websocket_memset(s, c, smax);
            // Compiler barrier to prevent optimization removal
            __asm__ __volatile__(""
                                 :
                                 :
                                 : "memory");
        }
        return EINVAL;
    }

    // Set n bytes of memory to the value c
    websocket_memset(s, c, n);
    // Compiler barrier to prevent optimization removal
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");

    return 0;
}
#else
#define websocket_memset_s memset_s
#endif  // __STDC_LIB_EXT1__
#else
#define websocket_memset_s memset_s
#endif  // __APPLE

#endif
