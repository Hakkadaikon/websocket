#ifndef NOSTR_LINUX_X86_64_SIGACTION_H_
#define NOSTR_LINUX_X86_64_SIGACTION_H_

#include "../../../util/allocator.h"
#include "../../../util/types.h"
#include "../errno.h"
#include "../sigaction_def.h"
#include "asm.h"

extern void linux_x8664_restore_rt(void);

static inline int32_t linux_x8664_sigaction(const int32_t signum, struct sigaction* act, struct sigaction* oldact)
{
  act->sa_flags |= SA_RESTORER;
  act->sa_restorer = &linux_x8664_restore_rt;

  int32_t ret = linux_x8664_asm_syscall4(
    __NR_rt_sigaction,
    signum,
    act,
    oldact,
    sizeof(sigset_t));

  // var_dump_local(1, "sigaction ret:", ret);
  // var_dump_local(1, "signum:", signum);
  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

static inline int32_t linux_x8664_sigemptyset(sigset_t* set)
{
  websocket_memset_s(set, sizeof(sigset_t), 0x00, sizeof(sigset_t));
  return 1;
}

#endif
