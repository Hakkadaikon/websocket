#ifndef NOSTR_LINUX_X86_64_CLOSE_H_
#define NOSTR_LINUX_X86_64_CLOSE_H_

#include "../errno.h"
#include "./asm.h"

static inline int linux_x8664_close(const int fd)
{
    long ret = linux_x8664_asm_syscall1(
        __NR_close,
        fd);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
