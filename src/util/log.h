#ifndef NOSTR_LOG_H_
#define NOSTR_LOG_H_

#include <stdarg.h>
#include <unistd.h>

void    hex_dump(const void* data, size_t size);
ssize_t stdout_print(const char* str);
ssize_t stderr_print(const char* str);

#endif
