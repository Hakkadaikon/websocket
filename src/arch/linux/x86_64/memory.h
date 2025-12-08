#ifndef NOSTR_LINUX_X86_64_MEMORY_H_
#define NOSTR_LINUX_X86_64_MEMORY_H_

#include "../../../util/types.h"
#include "../errno.h"

static inline void* linux_x8664_memcpy(void* dest, const void* src, size_t size)
{
  if (dest == NULL || src == NULL || size == 0) {
    return NULL;
  }

  unsigned char*       d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;
  size_t               n = size;

  while (n--) {
    *d++ = *s++;
  }
  return dest;
}

static inline void* linux_x8664_memset(void* s, const int c, const size_t size)
{
  if (s == NULL && size != 0) {
    return NULL;
  }

  unsigned char* p = (unsigned char*)s;
  size_t         n = size;
  while (n--) {
    *p++ = (unsigned char)c;
  }

  return s;
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
static inline int32_t linux_x8664_memset_s(void* s, const size_t smax, const int32_t c, const size_t n)
{
  if (s == NULL && n != 0) {
    return EINVAL;
  }

  // If n is greater than smax, clear the entire buffer (if possible) and return an error
  if (n > smax) {
    if (s != NULL && smax > 0) {
      linux_x8664_memset(s, c, smax);
      // Compiler barrier to prevent optimization removal
      __asm__ volatile("" ::
                         : "memory");
    }
    return EINVAL;
  }

  // Set n bytes of memory to the value c
  linux_x8664_memset(s, c, n);
  // Compiler barrier to prevent optimization removal
  __asm__ volatile("" ::
                     : "memory");

  return 0;
}

#endif
