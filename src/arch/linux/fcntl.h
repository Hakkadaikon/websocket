#ifndef NOSTR_FCNTL_H_
#define NOSTR_FCNTL_H_

#define O_ACCMODE 0003
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02
#define O_CREAT 0100   // Not fcntl.
#define O_EXCL 0200    // Not fcntl.
#define O_NOCTTY 0400  // Not fcntl.
#define O_TRUNC 01000  // Not fcntl.
#define O_APPEND 02000
#define O_NONBLOCK 04000
#define O_NDELAY O_NONBLOCK
#define O_SYNC 04010000
#define O_FSYNC O_SYNC
#define O_ASYNC 020000
#define __O_LARGEFILE 0100000
#define __O_DIRECTORY 0200000
#define __O_NOFOLLOW 0400000
#define __O_CLOEXEC 02000000
#define __O_DIRECT 040000
#define __O_NOATIME 01000000
#define __O_PATH 010000000
#define __O_DSYNC 010000
#define __O_TMPFILE (020000000 | __O_DIRECTORY)

#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7
#define F_GETLK64 12
#define F_SETLK64 13
#define F_SETLKW64 14
#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define __F_SETOWN 8
#define __F_GETOWN 9

#define __F_SETSIG 10
#define __F_GETSIG 11
#define __F_SETOWN_EX
#define __F_GETOWN_EX
#define FD_CLOEXEC 1  // Actually anything with low bit set goes

#define F_RDLCK 0  // Read lock.
#define F_WRLCK 1  // Write lock.
#define F_UNLCK 2  // Remove lock.
#define F_EXLCK 4  // or 3
#define F_SHLCK 8  // or 4
#define __POSIX_FADV_DONTNEED 4
#define __POSIX_FADV_NOREUSE 5

#endif
