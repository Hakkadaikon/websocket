#include "sha1.h"

#include <openssl/sha.h>

void sha1(const char* restrict input, const size_t input_len, uint8_t* restrict output)
{
    SHA1((const unsigned char*)input, input_len, output);
}
