#ifndef ZIPINT_LIBRARY_HPP
#define ZIPINT_LIBRARY_HPP

#define self (*this)

#include <iostream> // Todo remove this include

#include <cstdint>
#include <type_traits>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <zconf.h>
#include <array>
#include <bitset>

class LittleBigEndian {
public:
    enum Endianness {
        LITTLE = 0,
        BIG = 1
    };

    LittleBigEndian() = delete;
private:

    static Endianness getEndianness() noexcept __attribute__((optimize(0))) {
        uint16_t temp = 0x0100;
        return static_cast<Endianness>(*reinterpret_cast<uint8_t *>(&temp));
    }

public:
    inline static const Endianness endianness = LittleBigEndian::getEndianness();
};

template<bool _signed=true, bool _escape=false, bool _dynamic_zip_int_type=false, std::uint16_t _header_type=0>
class ZipInt {
    static_assert(_header_type <= 1, "ZipInt _header_type can't be higher that 1");
    // Warning for the future if more _header_type were to be add, it can't be higher than 15!!

    using Zip = ZipInt<_signed, _escape, _header_type>;

    using SignedType = std::conditional_t<not _signed, const bool, bool>;
    using SizeType = std::conditional_t<_header_type == 0, uint32_t, uint64_t>;

    class detail {

        template <typename, typename T>
        using check = T;


        template <typename T>
        static auto find_size(const T &container, int) noexcept
            -> decltype(std::size(container))
        {
            return std::size(container);
        }

        template <typename T>
        static auto find_size(const T &container, long)
        {
            return std::count_if(container.begin(), container.end(), [] (const auto &) { return true; });
        }
        /*
        template<auto _fn, template<typename ...> typename _tupleA, typename ..._typename,
             template<typename ...> typename _tupleB, typename ..._args>
        static constexpr auto fn_declfn(_tupleA<_typename...>, _tupleB<_args...>)
            -> decltype(fn<_typename...>(std::declval<_args>()...))
        {
            return {};
        }

        template<auto _fn, typename _typename_tuple, typename _args_tuple>
        using declfn = decltype(fn_declfn<>());
         */ // C++ 20

    public:
        template <typename T, typename = void>
        struct has_write : std::false_type {};

        template <typename T>
        struct has_write<T,
            std::void_t<std::invoke_result_t<decltype(&T::write), T, const char *, int>>
        > : std::true_type {};

        template <typename T> inline static constexpr bool has_write_v = has_write<T>::value;

        template <typename T, typename = void>
        struct has_read : std::false_type {};

        template <typename T>
        struct has_read<T,
            std::void_t<std::invoke_result_t<decltype(&T::read), T,  char *, ptrdiff_t>>
        > : std::true_type {};

        template <typename T> inline static constexpr bool has_read_v = has_read<T>::value;

        template <typename T, typename = void>
        struct is_iterable : std::false_type {};

        template <typename T>
        struct is_iterable<T,
            std::void_t<decltype (
                begin(std::declval<T&>()) != end(std::declval<T&>()),
                ++std::declval<decltype(begin(std::declval<T&>()))&>(),
                *begin(std::declval<T&>()))
            >
        > : std::true_type {};

        template <typename T> inline static constexpr bool is_iterable_v = is_iterable<T>::value;

        template <typename T>
        [[nodiscard]] static decltype(auto) size(const T &container)
        {
            return detail::find_size(0);
        }
    };

    template <size_t _nb>
    class bits : private std::array<uint8_t, _nb / 8 + (_nb % 8 ? 1 : 0)> {
        static constexpr size_t _nb_bytes = _nb / 8 + (_nb % 8 ? 1 : 0);

    public:
        [[nodiscard]] bool get(size_t idx) const
        {
            return (self.operator[](idx / 8) << idx % 8) & 0x80;
        }

        bits &set(size_t idx, bool value)
        {
            uint8_t &byte = self.operator[](idx / 8);
            uint8_t newValue = value << (7 - (idx % 8));

            byte ^= newValue;
            byte |= newValue;
            return self;
        }
    };

    uint16_t nbPower;               // 2B
    union  {
        uint8_t head;           // 1B
        char c_head{};          // 1B
        char ard_head[0];       // 0B
    };                              // 1B
    SignedType isSigned;            // 1B   -> should round up to 4B
    SizeType dataSize;              // 4B or 8B, if sizeof(SizeType) is 4B than round up to 8B else round up to 16B



// Member

    template<typename _tp>
    static void _endianlessify(const _tp &data, bits<sizeof(_tp) * 8> &out)
    {
        if (LittleBigEndian::endianness == LittleBigEndian::BIG) {
            const char *it = reinterpret_cast<const char *>(&data);

            std::copy(it, it + sizeof(_tp), reinterpret_cast<char *>(&out));
        } else {
            const char *it = reinterpret_cast<const char *>(&data);

            std::reverse_copy(it, it + sizeof(_tp), reinterpret_cast<char *>(&out));
        }
    }

    template<typename _stream, typename _tp>
    void _zip(_stream &stream, const _tp &data)
    {
        bits<sizeof(_tp) * 8> enData;

        Zip::_endianlessify(data, enData);

/*        if constexpr (_signed) {

        }
        */

        enData.set(3, true);
        for (uint32_t i = 0; i < sizeof(_tp) * 8; ++i) {
            if (i && !(i % 8))
                std::cout << ':';
            std::cout << enData.get(i);
        }
        std::cout << std::endl;

    }


    template<typename _stream, bool _container>
    void _typeIt(_stream &stream)
    {
        if constexpr (_dynamic_zip_int_type) {
            static_assert(_header_type <= 15, "ERROR FOR DEV, It seems like a _header_type superior"
                                              " to 15 has been created, sadly for binary reason it "
                                              "can't happen");

            constexpr const uint8_t flag = (uint8_t(_signed) << uint8_t(7)) +
                                   (uint8_t(_escape) << uint8_t(6)) +
                                   (uint8_t(_dynamic_zip_int_type) << uint8_t(5)) +
                                   (uint8_t(_container) << uint8_t(4)) +
                                   (uint8_t(_header_type));

            stream.write(&flag, 1);
        }
    }

    template<typename _stream, typename _tp, std::enable_if_t<detail::template is_iterable_v<_stream>, int> = 0>
    void _write(_stream &stream, const _tp &data)
    {
        // The data is a container

        self.template _typeIt<_stream, true>(stream);
        for (const auto &obj : data)
            self._zip(stream, obj);
    }


    template<typename _stream, typename _tp>
    void _write(_stream &stream, const _tp &data)
    {
        self.template _typeIt<_stream, false>(stream);
        self._zip(stream, data);
    }


public:
    ZipInt()
        : nbPower(0), head(), isSigned(false), dataSize(0)
    {}

    template<typename _stream, typename _tp>
    auto write(_stream &stream, const _tp &data)
        -> std::enable_if_t<detail::template has_write_v<_stream>>
    {
        self._write(stream, data);
    }

    template<typename _tp>
    void write(int fd, const _tp &data)
    {
        struct {
            int fd;
            void write(const void *data, ptrdiff_t n) { ::write(fd, data, n); }
        } fdWriteContainer = fd;

        self.write(fdWriteContainer, data);
    }

    template<typename _tp>
    void write(FILE *file, const _tp &data)
    {
        struct {
            FILE *file;
            void write(const void *data, ptrdiff_t n) { ::fwrite(data, 1, n, file); }
        } fileWriteContainer = file;

        self.write(fileWriteContainer, data);
    }

private:
    inline static Zip _singleton;
public:
    [[nodiscard]] static Zip &Get()
    {
        return Zip::_singleton;
    }
};

#undef self

#endif //ZIPINT_LIBRARY_HPP