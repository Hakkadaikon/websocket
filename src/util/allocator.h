#ifndef NOSTR_ALLOCATOR_H_
#define NOSTR_ALLOCATOR_H_

#include <alloca.h>
#include <stdlib.h>

#define ALLOCATE_STACK
// #define ALLOCATE_HEAP

#if defined(ALLOCATE_STACK)
#define websocket_alloc(size) alloca(size)
#define websocket_free(addr)
#elif defined(ALLOCATE_HEAP)
#define websocket_alloc(size) malloc(size)
#define websocket_free(addr) free(addr)
#endif

#endif
