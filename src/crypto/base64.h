#ifndef NOSTR_BASE64_H_
#define NOSTR_BASE64_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../util/string.h"

#define is_base64_char(c) (is_lower_char(c) || is_upper_char(c) || is_number_char(c) || c == '+' || c == '/')
static const char padding_char = '=';

static inline void base64_encode(const uint8_t* input, size_t input_len, char* output, const size_t output_len)
{
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t     input_offset;
    size_t     output_offset;

    for (
        input_offset = 0, output_offset = 0;
        (input_offset < input_len) && (output_offset < output_len - 4);
        input_offset += 3, output_offset += 4) {
        uint32_t octet_a = ((input_offset + 0) < input_len) ? input[input_offset + 0] : 0;
        uint32_t octet_b = ((input_offset + 1) < input_len) ? input[input_offset + 1] : 0;
        uint32_t octet_c = ((input_offset + 2) < input_len) ? input[input_offset + 2] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[output_offset + 0] = base64_chars[(triple >> 18) & 0x3F];
        output[output_offset + 1] = base64_chars[(triple >> 12) & 0x3F];
        output[output_offset + 2] = base64_chars[(triple >> 6) & 0x3F];
        output[output_offset + 3] = base64_chars[(triple >> 0) & 0x3F];
    }

    int input_len_mod = input_len % 3;
    int padding_count = (input_len_mod) ? (3 - input_len_mod) : 0;
    for (size_t i = 1; i <= padding_count; i++) {
        output[output_offset - i] = padding_char;
    }

    output[output_offset] = '\0';
}

static inline bool is_base64(const char* str)
{
    size_t len = strlen(str);
    if (len < 2) {
        return false;
    }

    size_t padding_count = 0;
    for (size_t i = 1; i <= 2; i++) {
        if (str[len - i] == padding_char) {
            padding_count++;
        }
    }

    size_t base64_len = len - padding_count;

    // base64 char check
    for (size_t i = 0; i < base64_len; i++) {
        if (!is_base64_char(str[i])) {
            return false;
        }
    }

    return true;
}

#endif
