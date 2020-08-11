//
// Created by Ail on 11/08/2020.
//

#include <gtest/gtest.h>
#include "ZipIntTester.hpp"

TEST(test_container_default, basic_test)
{
    {
        auto list = {0u, 1u, 2u, 3u, 4u};
        EXPECT_EQ("00000101:00000000:00000001:00000010:00000011:00000100", ZipIntTester<false>::Test(list));
    }
    {
        auto list = {0, 1, 2, 3, 4};
        EXPECT_EQ("00000101:00000000:00000001:00000010:00000011:00000100", ZipIntTester<true>::Test(list));
    }
    {
        std::initializer_list<uint32_t> list{};
        EXPECT_EQ("00000000", ZipIntTester<false>::Test(list));
    }
    {
        std::initializer_list<int32_t> list{};
        EXPECT_EQ("00000000", ZipIntTester<true>::Test(list));
    }
}