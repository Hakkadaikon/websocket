#ifndef NOSTR_STRING_H_
#define NOSTR_STRING_H_

#include "../util/types.h"
#ifdef __APPLE__
#include <stddef.h>
#else
#endif

#define ptr_value(ptr) (*ptr)
#define is_null(str) (str == NULL)
#define is_empty_value(c) (c == '\0')
#define is_empty(str) (is_empty_value(ptr_value(str)))
#define is_null_or_empty(str) (is_null(str) || is_empty(str))
#define is_line_break(c) ((c == '\r') || (c == '\n'))
#define is_white_space(c) \
    ((c == ' ') ||        \
     (c == '\t') ||       \
     (c == '\r') ||       \
     (c == '\n') ||       \
     (c == '\v') ||       \
     (c == '\f'))
#define is_lower_char(c) ((c >= 'a') && (c <= 'z'))
#define is_upper_char(c) ((c >= 'A') && (c <= 'Z'))
#define is_number_char(c) ((c >= '0') && (c <= '9'))

static inline bool is_utf8_white_space(const char* str)
{
    // UTF-8: 0xE3 0x80 0x80
    return (str[0] == '\xE3') &&
           (str[1] == '\x80') &&
           (str[2] == '\x80');
}

static inline bool compare_case_sensitive(const char a, const char b)
{
    if (a != b) {
        return false;
    }

    return true;
}

static inline bool compare(const char a, const char b)
{
    char lower_a = (a >= 'A' && a <= 'Z') ? (a + 'a' - 'A') : (a);
    char lower_b = (b >= 'A' && b <= 'Z') ? (b + 'a' - 'A') : (b);

    if (lower_a != lower_b) {
        return false;
    }

    return true;
}

static inline bool is_compare_str(const char* str1, const char* str2, const size_t str1capacity, const size_t str2capacity, const bool case_sensitive)
{
    size_t capacity = (str1capacity > str2capacity) ? str2capacity : str1capacity;
    if (capacity == 0) {
        return false;
    }

    typedef bool (*PCompareFunc)(const char a, const char b);
    PCompareFunc compare_func = (case_sensitive) ? compare_case_sensitive : compare;

    for (size_t i = 0; i < capacity; i++) {
        if (!compare_func(str1[i], str2[i])) {
            return false;
        }
    }

    return true;
}

static inline int search_str(const char* base, const size_t base_len, const char* target, const size_t target_len, const bool case_sensitive)
{
    if (is_null_or_empty(base) || base_len <= 0 || target_len > base_len) {
        return -1;
    }

    if (is_null_or_empty(target) || target_len <= 0) {
        return -1;
    }

    for (size_t base_pos = 0; base_pos <= (base_len - target_len); base_pos++) {
        if (is_compare_str(&base[base_pos], target, base_len, target_len, case_sensitive)) {
            return base_pos;
        }
    }

    return -1;
}

static inline bool is_contain_str(const char* base, const size_t base_len, const char* target, const size_t target_len, const bool case_sensitive)
{
    return (search_str(base, base_len, target, target_len, case_sensitive) != -1);
}

static inline int skip_white_space(const char* buffer, const size_t buffer_size)
{
    if (is_null(buffer) || buffer_size <= 0) {
        return -1;
    }

    size_t pos = 0;
    while (!is_empty(&buffer[pos]) && pos < buffer_size) {
        char current = ptr_value(&buffer[pos]);
        if (is_white_space(current)) {
            pos++;
            continue;
        }

        if (pos + 2 < buffer_size) {
            if (is_utf8_white_space(&buffer[pos])) {
                pos += 3;
                continue;
            }
        }

        return pos;
    }

    return pos;
}

static inline int skip_word(const char* buffer, const size_t buffer_size)
{
    for (size_t i = 0; i < buffer_size; i++) {
        if (is_white_space(buffer[i])) {
            return i;
        }

        if (is_empty_value(buffer[i])) {
            return i;
        }

        if (i + 2 >= buffer_size) {
            continue;
        }

        if (is_utf8_white_space(&buffer[i])) {
            return i;
        }
    }

    return buffer_size;
}

static inline int skip_next_line(const char* buffer, const size_t buffer_len)
{
    if (is_null(buffer) || buffer_len < 2) {
        return 0;
    }

    size_t buffer_pos = 0;
    while (buffer[buffer_pos] != '\0' && buffer_pos < buffer_len - 1) {
        char current = buffer[buffer_pos];
        char next    = buffer[buffer_pos + 1];

        if (current == '\n') {
            return buffer_pos + 1;
        }

        if (next == '\n') {
            return buffer_pos + 2;
        }

        buffer_pos++;
    }

    return buffer_pos;
}

static inline int skip_token(const char* buffer, const size_t buffer_size, const char token)
{
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == token) {
            return i;
        }
    }

    return -1;
}

static size_t inline get_str_len(const char* str)
{
    int len = 0;
    while (str[len++] != '\0')
        ;

    return len - 1;
}

static size_t inline get_str_nlen(const char* str, const size_t capacity)
{
    int len = 0;
    while (str[len++] != '\0' && len < capacity)
        ;

    return len - 1;
}

#endif
