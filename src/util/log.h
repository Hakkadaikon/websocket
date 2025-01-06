#ifndef NOSTR_LOG_H_
#define NOSTR_LOG_H_

#include <stdarg.h>
#include <unistd.h>

typedef enum {
    LOGLEVEL_DEBUG   = 0,
    LOGLEVEL_INFO    = 1,
    LOGLEVEL_ERROR   = 2,
    LOGLEVEL_NOTHING = 3
} LOG_LEVEL;

void set_log_level(LOG_LEVEL level);
void hex_dump(const void* data, size_t size);
void log_dump(const int fd, const char* str);
void log_debug(const char* str);
void log_info(const char* str);
void log_error(const char* str);
void var_dump(const int fd, const char* str, int value);
void var_debug(const char* str, int value);
void var_info(const char* str, int value);
void var_error(const char* str, int value);

#endif
