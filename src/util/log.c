#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "./log.h"
#include <stddef.h>
#include <stdatomic.h>
#include <stdbool.h>

_Atomic static LOG_LEVEL current_level = LOG_LEVEL_DEBUG;

static inline bool is_dump_log(LOG_LEVEL level)
{
    if (level >= current_level) {
        return true;
    }

    return false;
}

static inline int calc_digit(int value)
{
    int is_negative = (value < 0);
    if (is_negative) {
        value = -value;
    }

    for (int i = 10, j = 1; i < 1000000000; i *= 10, j++) {
        if (value < i) {
            return j;
        }
    }

    return 10 + is_negative;
}

static inline size_t safe_itoa(int value, char* restrict buffer, size_t buffer_capacity)
{
    int   digit       = calc_digit(value);
    char* end         = buffer + digit;
    char* current     = end;
    int   is_negative = (value < 0);

    if (is_negative) {
        value = -value;
    }

    *current-- = '\0';
    do {
        *current-- = '0' + (value % 10);
        value /= 10;
    } while (value > 0 && current >= buffer);

    if (is_negative && current >= buffer) {
        *current-- = '-';
    }
    return end - (current + 1);
}

void set_log_level(LOG_LEVEL level)
{
    current_level = level;
}

void hex_dump(const void* restrict data, size_t size)
{
    const char* byte_data = (const char*)data;

    for (size_t i = 0; i < size; i++) {
        printf("%02X ", byte_data[i]);

        if ((i + 1) % 16 == 0) {
            putchar('\n');
        }
    }

    if (size % 16 != 0) {
        putchar('\n');
    }

    fflush(stdout);
}

void log_dump(const int fd, const char* str)
{
    (void)write(fd, str, strlen(str));
}

void log_debug(const char* restrict str)
{
    if (!is_dump_log(LOG_LEVEL_DEBUG)) {
        return;
    }

    log_dump(STDOUT_FILENO, str);
}

void log_info(const char* restrict str)
{
    if (!is_dump_log(LOG_LEVEL_INFO)) {
        return;
    }

    log_dump(STDOUT_FILENO, str);
}

void log_error(const char* restrict str)
{
    if (!is_dump_log(LOG_LEVEL_ERROR)) {
        return;
    }

    return log_dump(STDERR_FILENO, str);
}

void var_dump(const int fd, const char* restrict str, int value)
{
    (void)write(fd, str, strlen(str));

    char   buffer[32];
    size_t buffer_size      = safe_itoa(value, buffer, sizeof(buffer));
    buffer[buffer_size]     = '\n';
    buffer[buffer_size + 1] = '\0';

    (void)write(fd, buffer, buffer_size + 1);
}

void var_debug(const char* restrict str, int value)
{
    if (!is_dump_log(LOG_LEVEL_DEBUG)) {
        return;
    }

    var_dump(STDOUT_FILENO, str, value);
}

void var_info(const char* restrict str, int value)
{
    if (!is_dump_log(LOG_LEVEL_INFO)) {
        return;
    }

    var_dump(STDOUT_FILENO, str, value);
}

void var_error(const char* restrict str, int value)
{
    if (!is_dump_log(LOG_LEVEL_ERROR)) {
        return;
    }

    var_dump(STDERR_FILENO, str, value);
}
