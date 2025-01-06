#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "./log.h"

void hex_dump(const void* restrict data, size_t size)
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

ssize_t stdout_print(const char* str)
{
    return write(STDOUT_FILENO, str, strlen(str) + 1);
}

ssize_t stderr_print(const char* str)
{
    return write(STDERR_FILENO, str, strlen(str) + 1);
}
