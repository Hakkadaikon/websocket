#include "./signal.h"

#include "../arch/sigaction.h"
#include "allocator.h"

static bool rise_signal = false;

static void signal_handler(int signum);
bool        is_rise_signal();
bool        signal_init();

bool signal_init()
{
    struct sigaction sa;
    websocket_memset_s(&sa, sizeof(sa), 0x00, sizeof(sa));
    sa.sa_handler = signal_handler;
    internal_sigemptyset(&sa.sa_mask);

    int signals[] = {SIGHUP, SIGINT, SIGTERM};

    for (int i = 0; i < (sizeof(signals) / sizeof(signals[0])); i++) {
        if (internal_sigaction(signals[i], &sa, (void*)0) == -1) {
            return false;
        }
    }

    return true;
}

static void signal_handler(int signum)
{
    rise_signal = true;
}

bool is_rise_signal()
{
    return rise_signal;
}
