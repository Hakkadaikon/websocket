#ifndef NOSTR_LOG_H_
#define NOSTR_LOG_H_

#include <stdarg.h>
#include <unistd.h>

// void set_log_level(LOG_LEVEL level);
// #define LOG_LEVEL_DEBUG
// #define LOG_LEVEL_INFO
// #define LOG_LEVEL_ERROR
//
void log_dump_local(const int fd, const char* str);
void var_dump_local(const int fd, const char* str, int value);
void str_dump_local(const int fd, const char* str, const char* value);

#if !defined(LOG_LEVEL_DEBUG) && !defined(LOG_LEVEL_INFO) && !defined(LOG_LEVEL_ERROR)
#define LOG_LEVEL_ERROR
#endif

#ifdef LOG_LEVEL_DEBUG

#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str) log_dump(STDOUT_FILENO, str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_INFO)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_ERROR)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif
#define hex_dump(data, size)
#define log_dump(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str)
#define var_dump(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value)
#endif

#endif
