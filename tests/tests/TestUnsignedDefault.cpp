//
// Created by Ail on 01/08/2020.
//

#include <gtest/gtest.h>
#include "ZipIntTester.hpp"

using Zip = ZipIntTester<false>;


TEST(test_unsinged_default, int_edgy_cases)
{
    EXPECT_EQ("11110000:01111111:11111111:11111111:11111111", Zip::Test(uint32_t(std::numeric_limits<int>::max())));
    EXPECT_EQ("11110000:10000000:00000000:00000000:00000000", Zip::Test(uint32_t(std::numeric_limits<int>::max()) + 1u));
    EXPECT_EQ("11110000:11111111:11111111:11111111:11111111", Zip::Test(std::numeric_limits<unsigned int>::max()));

    EXPECT_EQ("00000000", Zip::Test(0));
    EXPECT_EQ("00000001", Zip::Test(1));
    EXPECT_EQ("00000010",Zip::Test(2));
    EXPECT_EQ("01111111",Zip::Test(127));
    EXPECT_EQ("10000000:10000000",Zip::Test(128));
}

TEST(test_unsinged_default, int_middel_test)
{
    EXPECT_EQ("00110100",Zip::Test(52));
    EXPECT_EQ("00001100",Zip::Test(12));
    EXPECT_EQ("10000000:11100011",Zip::Test(227));
    EXPECT_EQ("10000000:11100100",Zip::Test(228));
    EXPECT_EQ("11110000:00101010:01111111:11101001:11110111",Zip::Test(713026039));
}

TEST(test_unsinged_default, long_edgy_cases)
{
    EXPECT_EQ("11111111:01111111:11111111:11111111:11111111:11111111:11111111:11111111:11111111", Zip::Test(uint64_t(std::numeric_limits<int64_t>::max())));
    EXPECT_EQ("11111111:10000000:10000000:00000000:00000000:00000000:00000000:00000000:00000000:00000000", Zip::Test(uint64_t(std::numeric_limits<int64_t>::max()) + 1));
    EXPECT_EQ("11111111:10000000:11111111:11111111:11111111:11111111:11111111:11111111:11111111:11111111", Zip::Test(std::numeric_limits<uint64_t>::max()));
    EXPECT_EQ("11111111:01111111:11111111:11111111:11111111:11111111:11111111:11111111:11111111", Zip::Test(std::numeric_limits<uint64_t>::max() >> 1ul));

    EXPECT_EQ("11110000:01111111:11111111:11111111:11111111", Zip::Test(uint64_t(std::numeric_limits<int32_t>::max())));
    EXPECT_EQ("11110000:10000000:00000000:00000000:00000000", Zip::Test(uint64_t(std::numeric_limits<int32_t>::max()) + 1ul));
    EXPECT_EQ("11110001:00000000:00000000:00000000:00000000", Zip::Test((uint64_t(std::numeric_limits<int32_t>::max()) + 1ul) << 1u));


    EXPECT_EQ("00000000", Zip::Test(0l));
    EXPECT_EQ("00000001", Zip::Test(1l));
    EXPECT_EQ("00000010",Zip::Test(2l));
    EXPECT_EQ("01111111",Zip::Test(127l));
    EXPECT_EQ("10000000:10000000",Zip::Test(128l));
}

TEST(test_unsinged_default, long_middel_test)
{
    EXPECT_EQ("11110000:10010000:01010100:10000010:11001001",Zip::Test(2421457609l));
    EXPECT_EQ("11110001:01100010:01010010:10000110:01000011",Zip::Test(5944542787l));
}