//
// Created by Ail on 11/08/2020.
//

#include <gtest/gtest.h>
#include "ZipIntTester.hpp"

TEST(test_read_default_32_128_128, basic_test)
{

    for (int32_t value = -128; value < 128; value++) {
        MyStream stream;

        ZipInt<true>::Get().write(stream, value);

        std::string tem = stream.str();

        std::istringstream istream {stream.str()};

        //istream.read
        int32_t result = ZipInt<true>::Get().readOnce<MyStream, int32_t>(stream);

        ASSERT_EQ(value, result);
    }
}

TEST(test_read_default_32_16m_16m, basic_test)
{

    for (int32_t value = std::numeric_limits<uint16_t>::min(); value < std::numeric_limits<int16_t>::max(); value++) {
        MyStream stream;

        ZipInt<true>::Get().write(stream, value);

        std::string tem = stream.str();

        std::istringstream istream {stream.str()};

        //istream.read
        int32_t result = ZipInt<true>::Get().readOnce<MyStream, int32_t>(stream);

        ASSERT_EQ(value, result);
    }
}

TEST(test_read_default_32_32m_32m, basic_test)
{
    int64_t cnt = 0;
    MyStreamLowResources<32> stream;

    std::cout << "PLEASE USE AN OPTIMISATION FLAG FOR THIS TEST" << std::endl;
    for (int32_t value = std::numeric_limits<int32_t>::min(); value < std::numeric_limits<int32_t>::max(); value++) {
        stream.reset();

        ZipInt<true>::Get().write(stream, value);

        int32_t result = ZipInt<true>::Get().readOnce<MyStreamLowResources<32>, int32_t>(stream);

        ASSERT_EQ(value, result);

        if ((value & std::numeric_limits<uint16_t>::max()) == std::numeric_limits<uint16_t>::max()) {
            cnt++;
            std::cout << cnt << '/' << std::numeric_limits<uint16_t>::max() << std::endl;
        }
    }
}

TEST(test_read_default_u32_0_256, basic_test)
{

    for (uint32_t value = 0; value < 256; value++) {
        MyStream stream;

        ZipInt<true>::Get().write(stream, value);

        std::string tem = stream.str();

        std::istringstream istream {stream.str()};

        //istream.read
        uint32_t result = ZipInt<true>::Get().readOnce<MyStream, uint32_t>(stream);

        ASSERT_EQ(value, result);
    }
}

TEST(test_read_default_u32_256_16m, basic_test)
{

    for (uint32_t value = 256; value < std::numeric_limits<uint16_t>::max(); value++) {
        MyStream stream;

        ZipInt<true>::Get().write(stream, value);

        std::string tem = stream.str();

        std::istringstream istream {stream.str()};

        //istream.read
        uint32_t result = ZipInt<true>::Get().readOnce<MyStream, uint32_t>(stream);

        ASSERT_EQ(value, result);
    }
}

TEST(test_read_default_u32_16m_32m, basic_test)
{
    uint64_t cnt = 0;
    MyStreamLowResources<32> stream;

    std::cout << "PLEASE USE AN OPTIMISATION FLAG FOR THIS TEST" << std::endl;
    for (uint32_t value = std::numeric_limits<uint16_t>::max(); value < std::numeric_limits<uint32_t>::max(); value++) {
        stream.reset();

        ZipInt<true>::Get().write(stream, value);

        uint32_t result = ZipInt<true>::Get().readOnce<MyStreamLowResources<32>, uint32_t>(stream);

        ASSERT_EQ(value, result);

        if ((value & std::numeric_limits<uint16_t>::max()) == std::numeric_limits<uint16_t>::max()) {
            cnt++;
            std::cout << cnt << '/' << std::numeric_limits<uint16_t>::max() << std::endl;
        }
    }
}

TEST(test_read_default_u32_32m_32p16m, basic_test)
{
    MyStreamLowResources<32> stream;
    constexpr uint64_t start = std::numeric_limits<uint32_t>::max();
    constexpr uint64_t end = uint64_t(std::numeric_limits<uint32_t>::max()) + uint64_t(std::numeric_limits<uint16_t>::max());


    std::cout << "PLEASE USE AN OPTIMISATION FLAG FOR THIS TEST" << std::endl;
    for (uint64_t value = start; value < end; value++) {
        stream.reset();

        ZipInt<true>::Get().write(stream, value);

        uint64_t result = ZipInt<true>::Get().readOnce<MyStreamLowResources<32>, uint64_t>(stream);

        ASSERT_EQ(value, result);
    }
}