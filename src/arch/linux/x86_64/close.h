#ifndef NOSTR_LINUX_X86_64_CLOSE_H_
#define NOSTR_LINUX_X86_64_CLOSE_H_

#include "../errno.h"
#include "./asm.h"

static inline int linux_x8664_close(const int fd)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_close), "D"(fd)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
