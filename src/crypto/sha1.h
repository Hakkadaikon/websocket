#ifndef NOSTR_SHA1_H_
#define NOSTR_SHA1_H_

#include <stddef.h>
#include <stdint.h>

#define SHA1_DIGEST_LENGTH 20

void sha1(const char* input, const size_t input_len, uint8_t* output);

#endif
