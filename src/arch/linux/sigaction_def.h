#ifndef NOSTR_LINUX_SIGACTION_DEF_H_
#define NOSTR_LINUX_SIGACTION_DEF_H_

#include <stdint.h>

typedef unsigned long  sigset_t;
typedef void           signalfunc_t(int);
typedef void           restorefunc_t(void);
typedef signalfunc_t*  sighandler_t;
typedef restorefunc_t* sigrestore_t;

struct sigaction {
    sighandler_t  sa_handler;
    unsigned long sa_flags;
    sigrestore_t  sa_restorer;
    sigset_t      sa_mask;
};

#ifndef SA_RESTORER
#define SA_RESTORER 0x04000000
#endif

#ifndef SIGINT
#define SIGINT 2  // Interactive attention signal.
#endif

#ifndef SIGILL
#define SIGILL 4  // Illegal instruction.
#endif

#ifndef SIGABRT
#define SIGABRT 6  // Abnormal termination.
#endif

#ifndef SIGFPE
#define SIGFPE 8  // Erroneous arithmetic operation.
#endif

#ifndef SIGSEGV
#define SIGSEGV 11  // Invalid access to storage.
#endif

#ifndef SIGTERM
#define SIGTERM 15  // Termination request.
#endif

// Historical signals specified by POSIX.
#ifndef SIGHUP
#define SIGHUP 1  // Hangup.
#endif

#ifndef SIGQUIT
#define SIGQUIT 3  // Quit.
#endif

#ifndef SIGTRAP
#define SIGTRAP 5  // Trace/breakpoint trap.
#endif

#ifndef SIGKILL
#define SIGKILL 9  // Killed.
#endif

#ifndef SIGBUS
#define SIGBUS 10  // Bus error.
#endif

#ifndef SIGSYS
#define SIGSYS 12  // Bad system call.
#endif

#ifndef SIGPIPE
#define SIGPIPE 13  // Broken pipe.
#endif

#ifndef SIGALRM
#define SIGALRM 14  // Alarm clock.
#endif

// New(er) POSIX signals (1003.1-2008, 1003.1-2013).
#ifndef SIGURG
#define SIGURG 16  // Urgent data is available at a socket.
#endif

#ifndef SIGSTOP
#define SIGSTOP 17  // Stop, unblockable.
#endif

#ifndef SIGTSTP
#define SIGTSTP 18  // Keyboard stop.
#endif

#ifndef SIGCONT
#define SIGCONT 19  // Continue.
#endif

#ifndef SIGCHLD
#define SIGCHLD 20  // Child terminated or stopped.
#endif

#ifndef SIGTTIN
#define SIGTTIN 21  // Background read from control terminal.
#endif

#ifndef SIGTTOU
#define SIGTTOU 22  // Background write to control terminal.
#endif

#ifndef SIGPOLL
#define SIGPOLL 23  // Pollable event occurred (System V).
#endif

#ifndef SIGXCPU
#define SIGXCPU 24  // CPU time limit exceeded.
#endif

#ifndef SIGXFSZ
#define SIGXFSZ 25  // File size limit exceeded.
#endif

#ifndef SIGVTALRM
#define SIGVTALRM 26  // Virtual timer expired.
#endif

#ifndef SIGPROF
#define SIGPROF 27  // Profiling timer expired.
#endif

#ifndef SIGUSR1
#define SIGUSR1 30  // User-defined signal 1.
#endif

#ifndef SIGUSR2
#define SIGUSR2 31  // User-defined signal 2.
#endif

// Nonstandard signals found in all modern POSIX systems (including both BSD and Linux).
#ifndef SIGWINCH
#define SIGWINCH 28  // Window size change (4.3 BSD, Sun).
#endif

// Archaic names for compatibility.
#ifndef SIGIO
#define SIGIO SIGPOLL  // I/O now possible (4.2 BSD).
#endif

#ifndef SIGIOT
#define SIGIOT SIGABRT  // IOT instruction, abort() on a PDP-11.
#endif

#ifndef SIGCLD
#define SIGCLD SIGCHLD  // Old System V name
#endif

#endif
