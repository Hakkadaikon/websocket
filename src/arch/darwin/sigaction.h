#ifndef NOSTR_DARWIN_SIGACTION_H_
#define NOSTR_DARWIN_SIGACTION_H_

#include <signal.h>

static inline int darwin_sigaction(const int signum, struct sigaction* act, struct sigaction* oldact)
{
    return sigaction(signum, act, oldact);
}

static inline int darwin_sigemptyset(sigset_t* set)
{
    return sigemptyset(set);
}

#endif
