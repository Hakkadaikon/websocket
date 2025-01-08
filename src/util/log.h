#ifndef NOSTR_LOG_H_
#define NOSTR_LOG_H_

#include <stdarg.h>
#include <unistd.h>

//void set_log_level(LOG_LEVEL level);
void hex_dump_local(const void* data, size_t size);
void log_dump_local(const int fd, const char* str);
void log_debug_local(const char* str);
void log_info_local(const char* str);
void log_error_local(const char* str);
void var_dump_local(const int fd, const char* str, int value);
void var_debug_local(const char* str, int value);
void var_info_local(const char* str, int value);
void var_error_local(const char* str, int value);

#define LOG_LEVEL_ERROR

#ifdef LOG_LEVEL_DEBUG
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str) log_debug_local(str)
#define log_info(str) log_info_local(str)
#define log_error(str) log_error_local(str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value) var_debug_local(str, value)
#define var_info(str, value) var_info_local(str, value)
#define var_error(str, value) var_error_local(str, value)

#elif defined(LOG_LEVEL_INFO)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str) log_info_local(str)
#define log_error(str) log_error_local(str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value) var_info_local(str, value)
#define var_error(str, value) var_error_local(str, value)

#elif defined(LOG_LEVEL_ERROR)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str) log_error_local(str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value) var_error_local(str, value)

#elif defined(LOG_LEVEL_NOTHING)
#define hex_dump(data, size)
#define log_dump(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str)
#define var_dump(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value)
#endif

#endif
