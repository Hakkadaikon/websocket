#ifndef NOSTR_LINUX_X86_64_WRITE_H_
#define NOSTR_LINUX_X86_64_WRITE_H_

#include "../../../util/types.h"
#include "../errno.h"
#include "./asm.h"

static inline ssize_t linux_x8664_write(const int fd, const void* buf, const size_t count)
{
    long ret = linux_x8664_asm_syscall3(
        __NR_write,
        fd,
        buf,
        count);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
