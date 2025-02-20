#ifndef NOSTR_DARWIN_SIGACTION_H_
#define NOSTR_DARWIN_SIGACTION_H_

#include <signal.h>

#include "../../util/types.h"

static inline int32_t darwin_sigaction(const int32_t signum, struct sigaction* act, struct sigaction* oldact)
{
    return sigaction(signum, act, oldact);
}

static inline int32_t darwin_sigemptyset(sigset_t* set)
{
    return sigemptyset(set);
}

#endif
