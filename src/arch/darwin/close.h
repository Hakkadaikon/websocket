#ifndef NOSTR_DARWIN_CLOSE_H_
#define NOSTR_DARWIN_CLOSE_H_

#include <unistd.h>

static inline int darwin_close(int fd)
{
    return close(fd);
}

#endif
