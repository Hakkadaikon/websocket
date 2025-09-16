#ifndef NOSTR_UTIL_TYPES_H_
#define NOSTR_UTIL_TYPES_H_

#ifdef __APPLE__
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#else
#ifndef __cplusplus
#ifndef _INT8_T
#define _INT8_T
typedef char int8_t;
#endif

#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char uint8_t;
#endif

#ifndef _INT16_T
#define _INT16_T
typedef short int16_t;
#endif

#ifndef _UINT16_T
#define _UINT16_T
typedef unsigned short uint16_t;
#endif

#ifndef _INT32_T
#define _INT32_T
typedef int int32_t;
#endif

#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int uint32_t;
#endif

#ifndef _INT64_T
#define _INT64_T
typedef long long int64_t;
#endif

#ifndef _UINT64_T
#define _UINT64_T
typedef unsigned int uint64_t;
#endif

#ifndef _SIZE_T 
#define _SIZE_T 
typedef uint64_t size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int64_t  ssize_t;
#endif

#ifndef bool
#define bool int32_t
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1  // Standard output.
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2  // Standard error output.
#endif

#ifndef thread_local
# if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#  define thread_local _Thread_local
# else
#  define thread_local __thread
# endif
#endif

#endif
