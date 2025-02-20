#ifndef NOSTR_UTIL_TYPES_H_
#define NOSTR_UTIL_TYPES_H_

#include <stdint.h>
#ifdef __APPLE__
#include <stdbool.h>
#include <sys/types.h>
#else
typedef uint64_t size_t;
typedef int64_t  ssize_t;

#ifndef bool
#define bool int32_t
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
#endif
