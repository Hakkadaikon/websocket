#ifndef NOSTR_LINUX_X86_64_CLOSE_H_
#define NOSTR_LINUX_X86_64_CLOSE_H_

#include "../errno.h"
#include "./asm.h"

static inline int32_t linux_x8664_close(const int fd)
{
    int32_t ret = linux_x8664_asm_syscall1(
        __NR_close,
        fd);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
