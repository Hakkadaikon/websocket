#ifndef NOSTR_LINUX_X86_64_RECV_H_
#define NOSTR_LINUX_X86_64_RECV_H_

#include "../../../util/types.h"
#include "../errno.h"
#include "../sockaddr.h"
#include "./asm.h"

static inline ssize_t linux_x8664_recvfrom(
  const int32_t    sock_fd,
  void*            buf,
  const size_t     len,
  const int32_t    flags,
  struct sockaddr* src_addr,
  socklen_t*       addrlen)
{
  int32_t ret = linux_x8664_asm_syscall6(
    __NR_recvfrom,
    sock_fd,
    buf,
    len,
    flags,
    src_addr,
    addrlen);

  SYSCALL_SIZE_EARLY_RETURN(ret);
  return ret;
}

#endif
