#ifndef NOSTR_UTIL_TYPES_H_
#define NOSTR_UTIL_TYPES_H_

#ifdef __APPLE__
#include <stdbool.h>
#include <sys/types.h>
#else
typedef unsigned long size_t;
typedef long          ssize_t;

#ifndef bool
#define bool int
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
