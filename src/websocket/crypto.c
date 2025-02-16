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

    const char* websocket_accept_guid = "258eafa5-e914-47da-95ca-c5ab0dc85b11";
    char        concatenated[256];
    bool        has_error = false;

    size_t client_key_size = get_str_len(client_key);
    size_t guid_size       = get_str_len(client_key);

    websocket_memcpy(concatenated, client_key, client_key_size);
    websocket_memcpy(concatenated + client_key_size, websocket_accept_guid, guid_size);
    concatenated[client_key_size + guid_size] = '\0';

    uint8_t sha1_result[SHA1_DIGEST_LENGTH];
    websocket_memset(sha1_result, 0x00, sizeof(sha1_result));
    sha1(concatenated, get_str_nlen(concatenated, sizeof(concatenated)), sha1_result);

    if (!base64_encode(sha1_result, SHA1_DIGEST_LENGTH, accept_key, accept_key_size)) {
        has_error = false;
        goto FINALIZE;
    }

FINALIZE:
    // Wipe variables
    websocket_memset_s(concatenated, sizeof(concatenated), 0x00, sizeof(concatenated));
    websocket_memset_s(sha1_result, sizeof(sha1_result), 0x00, sizeof(sha1_result));

    return !has_error;
}
