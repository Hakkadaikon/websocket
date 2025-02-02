/*
 * @see https://github.com/clibs/sha1
 *
 * Test Vectors (from FIPS PUB 180-1)
 * "abc"
 *   A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
 * "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
 * A million repetitions of "a"
 *   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
 * #define LITTLE_ENDIAN * This should be #define'd already, if true.
 */

#include "sha1.h"

#include <stdint.h>
#include <string.h>

#include "../util/allocator.h"
#include "sha1_def.h"

void sha1Transform(uint32_t state[5], const uint8_t buffer[64]);
void sha1Init(Sha1Ctx* context);
void sha1Update(Sha1Ctx* context, const uint8_t* data, uint32_t len);
void sha1Final(uint8_t digest[20], Sha1Ctx* context);

void sha1(const char* input, const size_t input_len, uint8_t* output)
{
    Sha1Ctx  ctx;
    uint32_t ii;

    sha1Init(&ctx);
    for (ii = 0; ii < input_len; ii++) {
        sha1Update(&ctx, (const uint8_t*)input + ii, 1);
    }

    sha1Final(output, &ctx);
}

/**
 * @brief Hash a single 512-bit block. This is the core of the algorithm.
 */
void sha1Transform(uint32_t state[5], const uint8_t buffer[64])
{
    uint32_t     a, b, c, d, e;
    Char64Long16 block[1];  // use array to appear as a pointer

    memcpy(block, buffer, sizeof(Char64Long16));

    // Copy context->state[] to working vars
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // 4 rounds of 20 operations each. Loop unrolled.
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);

    // Add the working vars back into context.state[]
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    // Wipe variables
    a = b = c = d = e = 0;
    memset_s(block, sizeof(block), '\0', sizeof(block));
}

/**
 * @brief Initialize new context
 */
void sha1Init(Sha1Ctx* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

/**
 * @brief Run your data through this.
 */
void sha1Update(Sha1Ctx* context, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    uint32_t j;

    j = context->count[0];
    if ((context->count[0] += len << 3) < j) {
        context->count[1]++;
    }
    context->count[1] += (len >> 29);
    j = (j >> 3) & 63;
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        sha1Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            sha1Transform(context->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }
    memcpy(&context->buffer[j], &data[i], len - i);
}

/**
 * @brief Add padding and return the message digest.
 */
void sha1Final(uint8_t digest[SHA1_DIGEST_LENGTH], Sha1Ctx* context)
{
    uint32_t i;
    uint8_t  finalcount[8];
    uint8_t  c;

    for (i = 0; i < 8; i++) {
        // Endian independent
        finalcount[i] = (uint8_t)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }

    c = 0200;
    sha1Update(context, &c, 1);
    while ((context->count[0] & 504) != 448) {
        c = 0000;
        sha1Update(context, &c, 1);
    }

    // Should cause a Sha1Transform()
    sha1Update(context, finalcount, 8);

    for (i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        digest[i] = (uint8_t)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }

    // Wipe variables
    memset_s(context, sizeof(*context), '\0', sizeof(*context));
    memset_s(&finalcount, sizeof(finalcount), '\0', sizeof(finalcount));
}
