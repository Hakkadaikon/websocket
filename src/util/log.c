#include "../arch/write.h"
#include "../websocket/websocket.h"
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

static inline void hex_dump_char(char c)
{
    unsigned char uc           = (unsigned char)c;
    char          hex_digits[] = "0123456789ABCDEF";
    char          buf[3];

    buf[0] = hex_digits[uc / 16];
    buf[1] = hex_digits[uc % 16];
    buf[2] = ' ';

    internal_write(STDOUT_FILENO, buf, 3);
}

void hex_dump_local(const void* restrict data, size_t size)
{
    const char* byte_data = (const char*)data;

    for (size_t i = 0; i < size; i++) {
        hex_dump_char(byte_data[i]);

        if ((i + 1) % 16 == 0) {
            internal_write(STDOUT_FILENO, "\n", 1);
        }
    }

    if (size % 16 != 0) {
        internal_write(STDOUT_FILENO, "\n", 1);
    }
}

void log_dump_local(const int fd, const char* restrict str)
{
    if (is_null(str) || fd <= 0) {
        return;
    }

    size_t len = get_str_len(str);
    if (len == 0) {
        return;
    }

    (void)internal_write(fd, str, len);
}

void var_dump_local(const int fd, const char* restrict str, const int value)
{
    if (is_null(str) || fd <= 0) {
        return;
    }

    size_t len = get_str_len(str);
    if (len == 0) {
        return;
    }

    (void)internal_write(fd, str, len);

    char   buffer[32];
    size_t buffer_size      = safe_itoa(value, buffer, sizeof(buffer));
    buffer[buffer_size]     = '\n';
    buffer[buffer_size + 1] = '\0';

    (void)internal_write(fd, buffer, buffer_size + 1);
}

void str_dump_local(const int fd, const char* restrict str, const char* restrict value)
{
    if (is_null(str) || is_null(value) || fd <= 0) {
        return;
    }

    log_dump(fd, str);
    log_dump(fd, value);
    log_dump(fd, "\n");
}
