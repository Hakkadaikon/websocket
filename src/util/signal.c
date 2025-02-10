#include "signal.h"

#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>

_Atomic static bool rise_signal = false;

static void signal_handler(int signum);
bool        is_rise_signal();
void        signal_init();

void signal_init()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGHUP, &sa, (void*)0);
    sigaction(SIGINT, &sa, (void*)0);
    sigaction(SIGTERM, &sa, (void*)0);
}

static void signal_handler(int signum)
{
    rise_signal = true;
}

bool is_rise_signal()
{
    return rise_signal;
}
