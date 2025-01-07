#include "websocket.h"
#include "../crypt/base64.h"
#include "../util/log.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>  //sha1

static void sha1(const char* input, const size_t input_len, uint8_t* output);

bool generate_websocket_acceptkey(const char* client_key, const size_t accept_key_size, char* accept_key)
{
    if (client_key == NULL) {
        return false;
    }

    if (accept_key == NULL) {
        return false;
    }

    if (accept_key_size < 128) {
        return false;
    }

    const char* websocket_accept_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char        concatenated[256];

    //TODO: snprintf consumes a lot of stack space, so switch to a different algorithm.
    snprintf(concatenated, sizeof(concatenated), "%s%s", client_key, websocket_accept_guid);

    uint8_t sha1_result[SHA_DIGEST_LENGTH];
    sha1(concatenated, strnlen(concatenated, sizeof(concatenated)), sha1_result);
    base64_encode(sha1_result, SHA_DIGEST_LENGTH, accept_key, accept_key_size);
    return true;
}

static void sha1(const char* input, const size_t input_len, uint8_t* output)
{
    SHA1((const unsigned char*)input, input_len, output);
}
