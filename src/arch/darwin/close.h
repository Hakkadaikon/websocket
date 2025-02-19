#ifndef NOSTR_DARWIN_CLOSE_H_
#define NOSTR_DARWIN_CLOSE_H_

#include <unistd.h>

#include "../../util/types.h"

static inline int32_t darwin_close(int32_t fd)
{
    return close(fd);
}

#endif
