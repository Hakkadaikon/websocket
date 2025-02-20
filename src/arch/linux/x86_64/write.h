#ifndef NOSTR_LINUX_X86_64_WRITE_H_
#define NOSTR_LINUX_X86_64_WRITE_H_

#include "../../../util/types.h"
#include "../errno.h"
#include "./asm.h"

static inline ssize_t linux_x8664_write(const int32_t fd, const void* buf, const size_t count)
{
    int32_t ret = linux_x8664_asm_syscall3(
        __NR_write,
        fd,
        buf,
        count);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return (ssize_t)ret;
}

#endif
