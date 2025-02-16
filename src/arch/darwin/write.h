#ifndef NOSTR_DARWIN_WRITE_H_
#define NOSTR_DARWIN_WRITE_H_

#include <unistd.h>

static inline ssize_t darwin_write(const int fd, const void* buf, const size_t count)
{
    return write(fd, buf, count);
}

#endif
