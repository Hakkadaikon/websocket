#include <stdio.h>

#include "../crypto/base64.h"
#include "../crypto/sha1.h"
#include "../util/allocator.h"
#include "websocket_local.h"

bool generate_websocket_acceptkey(const char* client_key, const size_t accept_key_size, char* accept_key)
{
    if (is_null(client_key) || is_null(accept_key) || accept_key_size < 128) {
        log_error("Invalid WebSocket handshake request. Failed generate accept key\n");
        return false;
    }

    const char* websocket_accept_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char        concatenated[256];
    bool        has_error = false;

    // TODO: snprintf consumes a lot of stack space, so switch to a different algorithm.
    snprintf(concatenated, sizeof(concatenated), "%s%s", client_key, websocket_accept_guid);

    uint8_t sha1_result[SHA1_DIGEST_LENGTH];
    memset(sha1_result, 0x00, sizeof(sha1_result));
    sha1(concatenated, strnlen(concatenated, sizeof(concatenated)), sha1_result);

    if (!base64_encode(sha1_result, SHA1_DIGEST_LENGTH, accept_key, accept_key_size)) {
        has_error = false;
        goto FINALIZE;
    }

FINALIZE:
    // Wipe variables
    memset_s(concatenated, sizeof(concatenated), 0x00, sizeof(concatenated));
    memset_s(sha1_result, sizeof(sha1_result), 0x00, sizeof(sha1_result));

    return !has_error;
}
