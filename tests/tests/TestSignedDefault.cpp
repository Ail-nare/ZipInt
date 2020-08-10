//
// Created by Ail on 01/08/2020.
//

#include <gtest/gtest.h>
#include "ZipIntTester.hpp"

using Zip = ZipIntTester<true>;


TEST(test_singed_default, int_edgy_cases)
{
    EXPECT_EQ("11110100:00000000:00000000:00000000:00000000", Zip::Test(std::numeric_limits<int>::min()));
    EXPECT_EQ("11110100:00000000:00000000:00000000:00000101", Zip::Test(std::numeric_limits<int>::min() + 5));

    EXPECT_EQ("00000000", Zip::Test(0));
    EXPECT_EQ("00000001", Zip::Test(1));
    EXPECT_EQ("01111111", Zip::Test(-1));
    EXPECT_EQ("00000010",Zip::Test(2));
    EXPECT_EQ("01111110",Zip::Test(-2));
    EXPECT_EQ("00111111",Zip::Test(63));
    EXPECT_EQ("10000000:01000000",Zip::Test(64));
    EXPECT_EQ("01000000",Zip::Test(-64));
    EXPECT_EQ("10111111:10111111",Zip::Test(-65));
}

TEST(test_singed_default, int_middel_test)
{
    EXPECT_EQ("00110100",Zip::Test(52));
    EXPECT_EQ("00001100",Zip::Test(12));
    EXPECT_EQ("10000000:01111111",Zip::Test(127));
    EXPECT_EQ("10000000:10000000",Zip::Test(128));
    EXPECT_EQ("11110000:00101010:01111111:11101001:11110111",Zip::Test(713026039));
}

TEST(test_singed_default, long_edgy_cases)
{
    EXPECT_EQ("11111111:10100000:00000000:00000000:00000000:00000000:00000000:00000000:00000000:00000000", Zip::Test(std::numeric_limits<int64_t>::min()));
    EXPECT_EQ("11111111:10100000:00000000:00000000:00000000:00000000:00000000:00000000:00000000:00000101", Zip::Test(std::numeric_limits<int64_t>::min() + 5));
    EXPECT_EQ("11111111:01000000:00000000:00000000:00000000:00000000:00000000:00000000:00000000", Zip::Test(std::numeric_limits<int64_t>::min() / 2l));
    EXPECT_EQ("11111111:01100000:00000000:00000000:00000000:00000000:00000000:00000000:00000000", Zip::Test(std::numeric_limits<int64_t>::min() / 4l));

    EXPECT_EQ("11110111:10000000:00000000:00000000:00000000", Zip::Test(int64_t(std::numeric_limits<int32_t>::min())));
    EXPECT_EQ("11110111:10000000:00000000:00000000:00000111", Zip::Test(int64_t(std::numeric_limits<int32_t>::min()) + 7l));
    EXPECT_EQ("11110111:01111111:11111111:11111111:11111001", Zip::Test(int64_t(std::numeric_limits<int32_t>::min()) - 7l));
    EXPECT_EQ("11110111:01111111:11111111:11111111:11111000", Zip::Test(int64_t(std::numeric_limits<int32_t>::min()) - 8l));


    EXPECT_EQ("11111111:10000000:01111111:11111111:11111111:11111111:11111111:11111111:11111111:11111111", Zip::Test(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ("11111111:00111111:11111111:11111111:11111111:11111111:11111111:11111111:11111111", Zip::Test(int64_t(std::numeric_limits<uint64_t>::max() >> 2u)));
    EXPECT_EQ("11111111:10000000:01111111:11111111:11111111:11111111:11111111:11111111:11111111:11111110", Zip::Test(std::numeric_limits<int64_t>::max() - 1));

    EXPECT_EQ("11110000:01111111:11111111:11111111:11111111", Zip::Test(uint64_t(std::numeric_limits<int32_t>::max())));
    EXPECT_EQ("11110000:10000000:00000000:00000000:00000000", Zip::Test(uint64_t(std::numeric_limits<int32_t>::max()) + 1ul));
    EXPECT_EQ("11110001:00000000:00000000:00000000:00000000", Zip::Test((uint64_t(std::numeric_limits<int32_t>::max()) + 1ul) << 1u));


    EXPECT_EQ("00000000", Zip::Test(0l));
    EXPECT_EQ("00000001", Zip::Test(1l));
    EXPECT_EQ("01111111", Zip::Test(-1l));
    EXPECT_EQ("00000010",Zip::Test(2l));
    EXPECT_EQ("01111110",Zip::Test(-2l));
    EXPECT_EQ("00111111",Zip::Test(63l));
    EXPECT_EQ("10000000:01000000",Zip::Test(64l));
    EXPECT_EQ("01000000",Zip::Test(-64l));
    EXPECT_EQ("10111111:10111111",Zip::Test(-65l));
}

TEST(test_singed_default, long_middel_test)
{
    EXPECT_EQ("11110000:10010000:01010100:10000010:11001001",Zip::Test(2421457609l));
    EXPECT_EQ("11111111:10100000:00000000:00000000:00000000:00000000:10010000:01010100:10000010:11001001",Zip::Test(std::numeric_limits<int64_t>::min() + 2421457609l));
    EXPECT_EQ("11110001:01100010:01010010:10000110:01000011",Zip::Test(5944542787l));
    EXPECT_EQ("11111111:10100000:00000000:00000000:00000000:00000001:01100010:01010010:10000110:01000011",Zip::Test(std::numeric_limits<int64_t>::min() + 5944542787l));
}