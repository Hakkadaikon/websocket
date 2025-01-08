#include "./log.h"

#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "./string.h"

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

void hex_dump_local(const void* restrict data, size_t size)
{
    //TODO: I plan to prepare my own log (var_hex_info) and replace it with printf.
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

void log_dump_local(const int fd, const char* str)
{
    if (is_null(str) || fd <= 0) {
        return;
    }

    size_t len = strlen(str);
    if (len == 0) {
        return;
    }

    (void)write(fd, str, len);
}

void log_debug_local(const char* restrict str)
{
    log_dump(STDOUT_FILENO, str);
}

void log_info_local(const char* restrict str)
{
    log_dump(STDOUT_FILENO, str);
}

void log_error_local(const char* restrict str)
{
    return log_dump(STDERR_FILENO, str);
}

void var_dump_local(const int fd, const char* restrict str, int value)
{
    if (is_null(str) || fd <= 0) {
        return;
    }

    size_t len = strlen(str);
    if (len == 0) {
        return;
    }

    (void)write(fd, str, len);

    char   buffer[32];
    size_t buffer_size      = safe_itoa(value, buffer, sizeof(buffer));
    buffer[buffer_size]     = '\n';
    buffer[buffer_size + 1] = '\0';

    (void)write(fd, buffer, buffer_size + 1);
}

void var_debug_local(const char* restrict str, int value)
{
    var_dump(STDOUT_FILENO, str, value);
}

void var_info_local(const char* restrict str, int value)
{
    var_dump(STDOUT_FILENO, str, value);
}

void var_error_local(const char* restrict str, int value)
{
    var_dump(STDERR_FILENO, str, value);
}
