#ifndef NOSTR_LOG_H_
#define NOSTR_LOG_H_

#include <stdint.h>
#include <stdio.h>

static void hex_dump(const void* data, size_t size)
{
    const uint8_t* byte_data = (const uint8_t*)data;

    for (size_t i = 0; i < size; i++) {
        printf("%02X ", byte_data[i]);

        if ((i + 1) % 16 == 0) {
            putchar('\n');
        }
    }

    if (size % 16 != 0) {
        putchar('\n');
    }

    fflush(stdout);
}

#endif
