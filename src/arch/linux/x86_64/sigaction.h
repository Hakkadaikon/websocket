#ifndef NOSTR_LINUX_X86_64_SIGACTION_H_
#define NOSTR_LINUX_X86_64_SIGACTION_H_

#include "../../../util/allocator.h"
#include "../../../util/types.h"
#include "../errno.h"
#include "../sigaction_def.h"
#include "asm.h"

static void __restore_rt(void)
{
    __asm__ volatile(
        "movq %0, %%rax\n\t"
        "syscall"
        :
        : "i"(__NR_rt_sigreturn)
        : "rax");
    __builtin_unreachable();
}

#include <stdio.h>
static inline void dump_sigaction_registers(const char* label)
{
    volatile unsigned long reg_rax, reg_rdi, reg_rsi, reg_rdx, reg_r10, reg_rcx, reg_r11;
    //__asm__ volatile("lfence":::"memory");
    __asm__ volatile(
        "mov %%rax, %0\n\t"
        "mov %%rdi, %1\n\t"
        "mov %%rsi, %2\n\t"
        "mov %%rdx, %3\n\t"
        "mov %%r10, %4\n\t"
        "mov %%rcx, %5\n\t"
        "mov %%r11, %6\n\t"
        : "=r"(reg_rax), "=r"(reg_rdi), "=r"(reg_rsi),
          "=r"(reg_rdx), "=r"(reg_r10), "=r"(reg_rcx), "=r"(reg_r11)
        :
        : /* no clobber */);
    //__asm__ volatile("lfence":::"memory");
    printf("[%s] rax=0x%lx, rdi=0x%lx, rsi=0x%lx, rdx=0x%lx, r10=0x%lx, rcx=0x%lx, r11=0x%lx\n",
           label, reg_rax, reg_rdi, reg_rsi, reg_rdx, reg_r10, reg_rcx, reg_r11);
    fflush(stdout);
}

extern int linux_x8664_assembly_sigaction(const int signum, struct sigaction* act, struct sigaction* oldact, size_t sigsetsize);
static int linux_x8664_sigaction(const int signum, struct sigaction* act, struct sigaction* oldact)
{
    //act->sa_flags    = SA_RESTORER;
    //act->sa_restorer = __restore_rt;
    //act->sa_flags    = 0;
    //act->sa_restorer = 0;

    //struct sigaction oldact2;
    //printf("signum[%p:%d] act [%p] oldact[%p]\n", &signum, signum, act, oldact);
    //fflush(stdout);
    long ret = linux_x8664_assembly_sigaction(signum, act, oldact, sizeof(act->sa_mask.sig));

    //volatile register size_t r10_asm asm("r10") = sizeof(sigset_t);
    //__asm__ volatile(
    //    "syscall"
    //    : "=a"(ret)
    //    : "0"(__NR_rt_sigaction),
    //      "D"(signum),
    //      "S"(act),
    //      "d"(oldact),
    //      "r"(r10_asm)
    //    : "rcx", "r11", "memory");

    //__asm__ volatile(
    //    "syscall"
    //    : "=a"(ret)
    //    : "0"(__NR_rt_sigaction),
    //      "D"(signum),
    //      "S"(act),
    //      "d"(oldact),
    //      "r"(r10_asm)
    //    : "rcx", "r11", "memory");
    //asm volatile("lfence" ::: "memory");
    //dump_sigaction_registers("sigaction");

    //__asm__ volatile(
    //    "syscall"
    //    : "=a"(ret)
    //    : "0"(__NR_rt_sigaction),
    //      "D"(signum),
    //      "S"(act),
    //      "d"(oldact),
    //      "r"(r10_asm)
    //    : "rcx", "r11", "memory");

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        return -1;
    }

    return ret;
}

static inline int linux_x8664_sigemptyset(sigset_t* set)
{
    websocket_memset_s(set, sizeof(sigset_t), 0x00, sizeof(sigset_t));
    return 1;
}

#endif
