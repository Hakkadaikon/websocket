#ifndef NOSTR_INTERNAL_CLOSE_H_
#define NOSTR_INTERNAL_CLOSE_H_

#ifdef __APPLE__
#include "darwin/close.h"
#else
#include "linux/x86_64/close.h"
#endif

static inline int internal_close(int fd)
{
#ifdef __APPLE__
    return darwin_close(fd);
#else
    return linux_x8664_close(fd);
#endif
}

#endif
