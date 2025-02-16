#ifndef NOSTR_INTERNAL_WRITE_H_
#define NOSTR_INTERNAL_WRITE_H_

#ifdef __APPLE__
#else
#include "linux/x86_64/write.h"
#endif

static inline ssize_t internal_write(const int fd, const void* buf, const size_t count)
{
#ifdef __APPLE__
#else
    return linux_x8664_write(fd, buf, count);
#endif
}
#endif
