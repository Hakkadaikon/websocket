#ifndef NOSTR_LINUX_X86_64_OPTIMIZE_SOCKET_H_
#define NOSTR_LINUX_X86_64_OPTIMIZE_SOCKET_H_

#include "../errno.h"
#include "../fcntl.h"
#include "../sockaddr.h"
#include "../sockoption.h"
#include "./asm.h"

static inline int32_t linux_x8664_fcntl(const int32_t fd, const int32_t cmd, const int64_t arg)
{
  int32_t ret = linux_x8664_asm_syscall3(
    __NR_fcntl,
    fd,
    cmd,
    arg);

  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

static inline int32_t linux_x8664_setsockopt(
  const int32_t   sockfd,
  const int32_t   level,
  const int32_t   optname,
  const void*     optval,
  const socklen_t optlen)
{
  int32_t ret = linux_x8664_asm_syscall5(
    __NR_setsockopt,
    sockfd,
    level,
    optname,
    optval,
    optlen);

  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

#endif
