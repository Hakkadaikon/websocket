#ifndef NOSTR_LINUX_X86_64_WRITE_H_
#define NOSTR_LINUX_X86_64_WRITE_H_

#include "../../../util/types.h"
#include "./asm.h"
#include "./errno.h"

static inline ssize_t linux_x8664_write(const int fd, const void* buf, const size_t count)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_write), "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
