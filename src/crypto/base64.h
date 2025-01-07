#ifndef NOSTR_BASE64_H_
#define NOSTR_BASE64_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static inline void base64_encode(const uint8_t* input, size_t input_len, char* output, const size_t output_len)
{
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t     i;
    size_t     j;

    for (i = 0, j = 0; i < input_len && j < output_len - 4; i += 3) {
        uint32_t octet_a = i < input_len ? input[i] : 0;
        uint32_t octet_b = i + 1 < input_len ? input[i + 1] : 0;
        uint32_t octet_c = i + 2 < input_len ? input[i + 2] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = base64_chars[(triple >> 18) & 0x3F];
        output[j++] = base64_chars[(triple >> 12) & 0x3F];
        output[j++] = (i + 1 < input_len) ? base64_chars[(triple >> 6) & 0x3F] : '=';
        output[j++] = (i + 2 < input_len) ? base64_chars[triple & 0x3F] : '=';
    }

    output[j] = '\0';
}

static inline bool is_base64(const char* str)
{
    size_t len = strlen(str);

    if (len == 0) {
        return false;
    }

    size_t padding_count = 0;
    if (len >= 2 && str[len - 1] == '=') {
        padding_count++;
        if (str[len - 2] == '=') {
            padding_count++;
        }
    }

    for (size_t i = 0; i < len - padding_count; i++) {
        char c = str[i];
        if (
            !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) &&
            c != '+' &&
            c != '/') {
            return false;
        }
    }

    for (size_t i = len - padding_count; i < len; i++) {
        if (str[i] != '=') {
            return false;
        }
    }

    return true;
}

#endif
