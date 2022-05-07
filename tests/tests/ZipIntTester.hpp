//
// Created by Ail on 01/08/2020.
//

#ifndef ZIPINT_ZIPINTTESTER_HPP
#define ZIPINT_ZIPINTTESTER_HPP

#include "ZipInt.hpp"
#include <string>
#include <vector>
#include <deque>
#include <array>
#include <sstream>
#include <iomanip>

class MyStream {
    std::deque<uint8_t> _data;

    static std::string to_hex(uint8_t i)
    {
        std::stringstream stream;
        stream << std::setfill ('0') << std::setw(2) << std::hex << uint32_t(i);
        return stream.str();
    }

public:
    MyStream()=default;

    [[nodiscard]] std::string str() const
    {
        size_t size = this->_data.size();
        std::string result((size * 9) - 1, '0');

        for (size_t i = 0, j = 0; i < (size * 8); ++i) {
            if (i && !(i % 8)) {
                result[i + j] = ':';
                ++j;
            }
            if (uint8_t(this->_data[i / 8] << (i % 8u)) & 0x80u)
                result[i + j] = '1';
        }
        return result;
    }

    [[nodiscard]] std::string hex() const
    {
        std::stringstream stream;
        bool first = true;

        for (const auto &nb : this->_data) {
            if (!first)
                stream << ':';
            else
                first = false;
            stream << MyStream::to_hex(nb);
        }
        return stream.str();
    }

    void write(const void *data, int size)
    {
        const auto *udata = static_cast<const uint8_t *>(data);

        this->_data.insert(this->_data.end(), udata, udata + size);
    }

    ssize_t read(void *data, int size)
    {
        auto *udata = static_cast<uint8_t *>(data);

        std::copy(this->_data.cbegin(), this->_data.cbegin() + size, udata);
        this->_data.erase(this->_data.cbegin() + size - 1);
        return size;
    }
};

template <size_t _array_size>
class MyStreamLowResources {
    std::array<uint8_t, _array_size> _data;
    ptrdiff_t start = 0;
    ptrdiff_t last = 0;

    static std::string to_hex(uint8_t i)
    {
        std::stringstream stream;
        stream << std::setfill ('0') << std::setw(2) << std::hex << uint32_t(i);
        return stream.str();
    }

public:
    MyStreamLowResources()=default;

    [[nodiscard]] std::string str() const
    {
        size_t size = this->size();
        std::string result((size * 9) - 1, '0');

        for (size_t i = 0, j = 0; i < (size * 8); ++i) {
            if (i && !(i % 8)) {
                result[i + j] = ':';
                ++j;
            }
            auto ptr = this->begin();

            if (uint8_t(ptr[i / 8] << (i % 8u)) & 0x80u)
                result[i + j] = '1';
        }
        return result;
    }

    [[nodiscard]] std::string hex() const
    {
        std::stringstream stream;
        bool first = true;

        for (const auto &nb : *this) {
            if (!first)
                stream << ':';
            else
                first = false;
            stream << MyStreamLowResources::to_hex(nb);
        }
        return stream.str();
    }

    inline void reset() {
        this->start = 0;
        this->last = 0;
    }

    [[nodiscard]] inline size_t size() const {
        return this->last - this->last;
    }

    void write(const void *data, int size)
    {
        const auto *udata = static_cast<const uint8_t *>(data);

        std::copy_n(udata, size, this->begin());
        this->last += size;
    }

    ssize_t read(void *data, int size)
    {
        auto *udata = static_cast<uint8_t *>(data);

        std::copy(this->cbegin(), this->cbegin() + size, udata);
        this->start += size;
        return size;
    }

    typedef uint8_t* iterator;
    typedef const uint8_t* const_iterator;

    [[nodiscard]] inline iterator begin() { return &this->_data[this->start]; }
    [[nodiscard]] inline const_iterator begin() const { return &this->_data[this->start]; }
    [[nodiscard]] inline const_iterator cbegin() const { return &this->_data[this->start]; }
    [[nodiscard]] inline iterator end() { return &this->_data[this->last + 1]; }
    [[nodiscard]] inline const_iterator end() const { return &this->_data[this->last + 1];  }
    [[nodiscard]] inline const_iterator cend() const { return &this->_data[this->last + 1];  }
};

template<bool _signed=false, std::uint16_t _header_type=0, bool _dynamic_zip_int_type=false, bool _escape=false>
class ZipIntTester {

public:
    template <typename T>
    static std::string Test(const T &value)
    {
        MyStream stream;

        ZipInt<_signed, _header_type, _dynamic_zip_int_type, _escape>::Get().write(stream, value);

        return stream.str();
    }

    template <typename T>
    static std::string TestVoidptr(const T &value)
    {
        MyStream stream;

        ZipInt<_signed, _header_type, _dynamic_zip_int_type, _escape>::Get().write(stream, &value, sizeof(T));

        return stream.str();
    }

    template <typename T>
    static std::string TestHex(const T &value)
    {
        MyStream stream;

        ZipInt<_signed, _header_type, _dynamic_zip_int_type, _escape>::Get().write(stream, value);

        return stream.hex();
    }
};

template<typename... T>
struct same : std::false_type {};

template<>
struct same<> : std::true_type {};

template<typename T>
struct same<T> : std::true_type {};

template<typename T, typename... Ts>
struct same<T, T, Ts...> : same<T, Ts...> {};


template <typename type, size_t N>
class EndianNumber {
    static_assert(N >= 1, "N can't be inferior to 1");

    std::array<type, N> _data;

    template <size_t n, typename std::enable_if<n == 0, int>::type=0>
    constexpr void _set()
    {}

    template <size_t n, typename ...Ts, typename std::enable_if<same<type, Ts...>::value && (sizeof...(Ts) + 1 == n), int>::type=0>
    constexpr void _set(type nb, Ts ...rest)
    {
        if (LittleBigEndian::endianness == LittleBigEndian::BIG) {
            this->_data[N - n] = nb;
        } else {
            this->_data[n - 1] = nb;
        }
        this->_set<n - 1>(rest...);
    }

public:
    template <typename ...Ts, typename std::enable_if<same<type, Ts...>::value && (sizeof...(Ts) + 1 == N), int>::type=0>
    explicit constexpr EndianNumber(type nb, Ts ...rest)
    {
        this->_set<N>(nb, rest...);
    }

    [[nodiscard]] std::string str() const
    {
        std::string result;

        for (size_t i = 0; i < N; ++i)
            result += ':' + std::to_string(this->_data[i]);

        return result;
    }
};

#endif //ZIPINT_ZIPINTTESTER_HPP