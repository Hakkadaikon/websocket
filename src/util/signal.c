#include "signal.h"

#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

static void signal_handler(int signum)
{
    rise_signal = true;
}

bool is_rise_signal()
{
    return rise_signal;
}
