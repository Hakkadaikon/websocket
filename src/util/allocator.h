#ifndef NOSTR_ALLOCATOR_H_
#define NOSTR_ALLOCATOR_H_

#include "./types.h"
#ifdef __APPLE__
#include <alloca.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>  // for memset
#else
#include "../arch/linux/errno.h"
//extern void* alloca(size_t __size);
#undef  alloca
#undef  __alloca
#define alloca(size)   __alloca(size)
#define __alloca(size) __builtin_alloca(size)
#endif
#include "../arch/memory.h"

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

/**
 * websocket_memcpy
 */
static inline void* websocket_memcpy(void* dest, const void* src, size_t size)
{
    return internal_memcpy(dest, src, size);
}

/*
 * websocket_memset
 */
static inline void* websocket_memset(void* s, const int32_t c, const size_t size)
{
    return internal_memset(s, c, size);
}

/*
 * websocket_memset_s
 */
static inline int32_t websocket_memset_s(void* s, const size_t smax, const int32_t c, const size_t n)
{
    return internal_memset_s(s, smax, c, n);
}

#endif
