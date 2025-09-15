#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

extern "C" {
#include "crypto/base64.h"
}

static size_t RequiredBase64Capacity(size_t input_len)
{
    size_t blocks = (input_len + 2) / 3;
    return blocks * 4 + 1;
}

TEST(Base64Test, EncodeKnownVectors)
{
    struct Case {
        std::string input;
        std::string expected;
    };

    std::vector<Case> cases = {
        {"f", "Zg=="},
        {"hello", "aGVsbG8="},
        {"websocket", "d2Vic29ja2V0"},
    };

    for (const auto& tc : cases) {
        const size_t cap = RequiredBase64Capacity(tc.input.size());
        std::string  out;
        out.resize(cap);

        bool ok = base64_encode(
            reinterpret_cast<const uint8_t*>(tc.input.data()),
            static_cast<size_t>(tc.input.size()),
            &out[0],
            cap);
        ASSERT_TRUE(ok) << "encode failed for input: " << tc.input;

        // Ensure null-terminated
        ASSERT_EQ(out[tc.expected.size()], '\0');

        // Compare only the meaningful part (excluding null)
        out.resize(tc.expected.size());
        EXPECT_EQ(out, tc.expected) << "mismatch for input: " << tc.input;
    }
}

TEST(Base64Test, EncodeEmptyInputFails)
{
    char out[8] = {0};
    bool ok     = base64_encode(nullptr, 0, out, sizeof(out));
    EXPECT_FALSE(ok);
}

TEST(Base64Test, EncodeNullOutputFails)
{
    const uint8_t data[1] = {0x61};
    bool          ok      = base64_encode(data, 1, nullptr, 0);
    EXPECT_FALSE(ok);
}

TEST(Base64Test, EncodeInsufficientCapacityFailsForMin)
{
    const uint8_t data[1] = {0x61};  // 'a' -> YQ==
    // Capacity <= 4 should fail per implementation precondition
    char out4[4] = {0};
    EXPECT_FALSE(base64_encode(data, 1, out4, sizeof(out4)));

    char out5[5] = {0};
    EXPECT_TRUE(base64_encode(data, 1, out5, sizeof(out5)));
    EXPECT_STREQ(out5, "YQ==");
}

TEST(Base64Test, IsBase64Validation)
{
    std::vector<std::pair<std::string, bool>> cases = {
        {"Zg==", true},
        {"Zm9v", true},
        {"aGVsbG8=", true},
        // invalid: too short
        {"A", false},
        {"==", false},
        // invalid: contains non-base64 char
        {"Zm9v*", false},
        // invalid: padding in the wrong place or too many
        {"A===", false},
        {"====", false},
    };

    for (const auto& [s, expect] : cases) {
        EXPECT_EQ(is_base64(s.c_str()), expect) << s;
    }
}
