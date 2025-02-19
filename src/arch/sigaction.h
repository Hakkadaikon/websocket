#ifndef NOSTR_INTERNAL_SIGACTION_H_
#define NOSTR_INTERNAL_SIGACTION_H_

#ifdef __APPLE__
#include "darwin/sigaction.h"
#else
#include "linux/x86_64/sigaction.h"
#endif

static inline int internal_sigaction(const int signum, struct sigaction* act, struct sigaction* oldact)
{
#ifdef __APPLE__
    return darwin_sigaction(signum, act, oldact);
#else
    return linux_x8664_sigaction(signum, act, oldact);
#endif
}

static inline int internal_sigemptyset(sigset_t* set)
{
#ifdef __APPLE__
    return darwin_sigemptyset(set);
#else
    return linux_x8664_sigemptyset(set);
#endif
}

#endif
