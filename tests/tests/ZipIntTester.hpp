//
// Created by Ail on 01/08/2020.
//

#ifndef ZIPINT_ZIPINTTESTER_HPP
#define ZIPINT_ZIPINTTESTER_HPP

#include "ZipInt.hpp"
#include <string>

class MyStream {
    std::vector<uint8_t> data;

public:
    MyStream(): data() {};

    [[nodiscard]] std::string str() const
    {
        size_t size = this->data.size();
        std::string result((size * 9) - 1, '0');

        for (size_t i = 0, j = 0; i < (size * 8); ++i) {
            if (i && !(i % 8)) {
                result[i + j] = ':';
                ++j;
            }
            if (uint8_t(this->data[i / 8] << (i % 8u)) & 0x80u)
                result[i + j] = '1';
        }
        return result;
    }

    void write(const void *data, int size)
    {
        const uint8_t *udata = static_cast<const uint8_t *>(data);

        this->data.insert(this->data.end(), udata, udata + size);
    }
};

template<bool _signed=false, bool _escape=false, bool _dynamic_zip_int_type=false, std::uint16_t _header_type=0>
class ZipIntTester {

public:
    template <typename T>
    static std::string Test(const T &value)
    {
        MyStream stream;

        ZipInt<_signed, _escape, _dynamic_zip_int_type, _header_type>::Get().write(stream, value);

        return stream.str();
    }
};


#endif //ZIPINT_ZIPINTTESTER_HPP
