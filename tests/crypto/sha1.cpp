#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
#include "crypto/sha1.h"
}

namespace
{

static std::vector<uint8_t> HexToBytes(const std::string& hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);

    auto is_space = [](char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    };

    auto hexval = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
        return -1;
    };

    for (size_t i = 0; i < hex.size();) {
        // skip whitespace
        while (i < hex.size() && is_space(hex[i]))
            ++i;
        if (i >= hex.size()) break;
        if (i + 1 >= hex.size()) break;  // odd length after trimming; ignore last nibble
        char c1 = hex[i++];
        // skip whitespace between nibbles if any (robustness)
        while (i < hex.size() && is_space(hex[i]))
            ++i;
        if (i >= hex.size()) break;
        char c2 = hex[i++];
        int  v1 = hexval(c1);
        int  v2 = hexval(c2);
        if (v1 < 0 || v2 < 0) {
            // skip invalid pair
            continue;
        }
        out.push_back(static_cast<uint8_t>((v1 << 4) | v2));
    }
    return out;
}

static std::string BytesToHex(const uint8_t* bytes, size_t len)
{
    static const char* kHex = "0123456789abcdef";
    std::string        s;
    s.resize(len * 2);
    for (size_t i = 0; i < len; ++i) {
        s[2 * i + 0] = kHex[(bytes[i] >> 4) & 0xF];
        s[2 * i + 1] = kHex[(bytes[i] >> 0) & 0xF];
    }
    return s;
}

TEST(Sha1Test, KnownVector_abc)
{
    const std::string msg = "abc";
    const char*       expected_hex =
        "A9993E364706816ABA3E25717850C26C9CD0D89D";

    uint8_t digest[SHA1_DIGEST_LENGTH] = {0};
    sha1(msg.c_str(), msg.size(), digest);

    const auto expected = HexToBytes(expected_hex);
    ASSERT_EQ(expected.size(), static_cast<size_t>(SHA1_DIGEST_LENGTH));
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), digest))
        << "got: " << BytesToHex(digest, SHA1_DIGEST_LENGTH);
}

TEST(Sha1Test, KnownVector_abcdbcdecdefdefg)
{
    const std::string msg =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const char* expected_hex =
        "84983E441C3BD26EBAAE4AA1F95129E5E54670F1";

    uint8_t digest[SHA1_DIGEST_LENGTH] = {0};
    sha1(msg.c_str(), msg.size(), digest);

    const auto expected = HexToBytes(expected_hex);
    ASSERT_EQ(expected.size(), static_cast<size_t>(SHA1_DIGEST_LENGTH));
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), digest))
        << "got: " << BytesToHex(digest, SHA1_DIGEST_LENGTH);
}

TEST(Sha1Test, KnownVector_MillionA)
{
    const std::string msg(1000000, 'a');
    const char*       expected_hex =
        "34AA973CD4C4DAA4F61EEB2BDBAD27316534016F";

    uint8_t digest[SHA1_DIGEST_LENGTH] = {0};
    sha1(msg.c_str(), msg.size(), digest);

    const auto expected = HexToBytes(expected_hex);
    ASSERT_EQ(expected.size(), static_cast<size_t>(SHA1_DIGEST_LENGTH));
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), digest))
        << "got: " << BytesToHex(digest, SHA1_DIGEST_LENGTH);
}

TEST(Sha1Test, KnownVector_Empty)
{
    const std::string msg = "";
    const char*       expected_hex =
        "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709";  // SHA1("")

    uint8_t digest[SHA1_DIGEST_LENGTH] = {0};
    sha1(msg.c_str(), msg.size(), digest);

    const auto expected = HexToBytes(expected_hex);
    ASSERT_EQ(expected.size(), static_cast<size_t>(SHA1_DIGEST_LENGTH));
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), digest))
        << "got: " << BytesToHex(digest, SHA1_DIGEST_LENGTH);
}

}  // namespace
