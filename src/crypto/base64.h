#ifndef NOSTR_BASE64_H_
#define NOSTR_BASE64_H_

#include "../util/string.h"

#define base64_padding_char ('=')
#define base64_input_block_size 3
#define base64_output_block_size 4
#define base64_num_of_char_bit 6
#define is_base64_char(c) \
  (is_lower_char(c) || is_upper_char(c) || is_number_char(c) || c == '+' || c == '/')
#define base64_get_array_value(array, offset, capacity) \
  ((offset < capacity) ? array[offset] : 0)
#define base64_char_mask \
  ((1 << base64_num_of_char_bit) - 1)
#define base64_out_char_index(marge, input_index) \
  ((marge >> ((base64_output_block_size - input_index - 1) * base64_num_of_char_bit)) & base64_char_mask)

static inline bool base64_encode(const uint8_t* input, const size_t input_length, char* output, const size_t output_capacity)
{
  const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  size_t     input_offset;
  size_t     output_offset;

  // minimum input length   : 1byte
  // minimum output capacity: 5byte(4byte + \0)
  if (is_null(input) || is_null(output) || input_length <= 0 || output_capacity <= base64_output_block_size) {
    return false;
  }

  for (
    input_offset = 0, output_offset = 0;
    (input_offset < input_length) && (output_offset < output_capacity - base64_output_block_size);
    input_offset += base64_input_block_size, output_offset += base64_output_block_size) {
    uint32_t marge =
      (base64_get_array_value(input, input_offset + 0, input_length) << 16) |
      (base64_get_array_value(input, input_offset + 1, input_length) << 8) |
      (base64_get_array_value(input, input_offset + 2, input_length) << 0);

    output[output_offset + 0] = base64_chars[base64_out_char_index(marge, 0)];
    output[output_offset + 1] = base64_chars[base64_out_char_index(marge, 1)];
    output[output_offset + 2] = base64_chars[base64_out_char_index(marge, 2)];
    output[output_offset + 3] = base64_chars[base64_out_char_index(marge, 3)];
  }

  int32_t input_length_mod = input_length % base64_input_block_size;
  int32_t padding_count    = (input_length_mod) ? (base64_input_block_size - input_length_mod) : 0;
  for (size_t i = 1; i <= padding_count; i++) {
    output[output_offset - i] = base64_padding_char;
  }

  output[output_offset] = '\0';
  return true;
}

static inline bool is_base64(const char* str)
{
  size_t len = get_str_len(str);
  if (len < base64_output_block_size) {
    return false;
  }

  size_t padding_count = 0;
  for (size_t i = 1; i <= 2; i++) {
    if (str[len - i] == base64_padding_char) {
      padding_count++;
    }
  }

  size_t base64_length = len - padding_count;

  // base64 char check
  for (size_t i = 0; i < base64_length; i++) {
    if (!is_base64_char(str[i])) {
      return false;
    }
  }

  return true;
}

#endif
