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
#include <tuple>

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

class Escape {
public:
    static constexpr uint8_t value = 0xFF;//_signed ? 0b01111111 : 0b10111111;

    static constexpr bool isEscape(Escape)
    {
        return true;
    }

    template <typename _tp>
    static constexpr bool isEscape(const _tp &value)
    {
        //Won't be a constexpr in most cases
        return value == Escape::value;
    }

    constexpr Escape()=default;
};

template<bool _signed=false, bool _escape=false, bool _dynamic_zip_int_type=false, std::uint16_t _header_type=0>
class ZipInt;

class ZipHelper {
    using SizeTypes = std::tuple<uint32_t, uint64_t>;

    template <std::uint16_t _header_type>
    using SizeType = std::tuple_element_t<_header_type, SizeTypes>;


    template <size_t _nb>
    class bits : private std::array<uint8_t, _nb / 8 + (_nb % 8 ? 1 : 0)> {
        static constexpr size_t _nb_bytes = _nb / 8 + (_nb % 8 ? 1 : 0);

    public:
        [[nodiscard]] bool get(size_t idx) const
        {
            return (self.operator[](idx / 8) << idx % 8) & 0x80;
        }

        [[nodiscard]] uint8_t &getByte(size_t idx) const
        {
            return self.at(idx);
        }

        bits &set(size_t idx, bool value)
        {
            uint8_t &byte = self.operator[](idx / 8);
            uint8_t newValue = 0x80u >> (idx % 8);

            byte ^= newValue;
            if (value)
                byte |= newValue;
            return self;
        }

        [[nodiscard]] const uint8_t *cbyte_begin() const
        {
            return self.begin();
        }

        [[nodiscard]] const uint8_t *cbyte_end() const
        {
            return self.begin() + _nb_bytes;
        }
    };

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
            return ZipHelper::detail::find_size(container, 0);
        }
    };

    template <std::uint16_t _header_type>
    class ZipDataDefault {
    protected:
        // Variable
        SizeType<_header_type> size;
    };

    template<bool _signed, bool _escape, bool _dynamic_zip_int_type, std::uint16_t _header_type>
    class ZipData {
        friend ZipInt<_signed, _escape, _dynamic_zip_int_type, _header_type>;
    public:
        ZipData()=delete;
    };

    template<typename _tp>
    static void _endianlessify(const _tp &data, ZipHelper::bits<sizeof(_tp) * 8> &out)
    {
        if (LittleBigEndian::endianness == LittleBigEndian::BIG) {
            const char *it = reinterpret_cast<const char *>(&data);

            std::copy(it, it + sizeof(_tp), reinterpret_cast<char *>(&out));
        } else {
            const char *it = reinterpret_cast<const char *>(&data);

            std::reverse_copy(it, it + sizeof(_tp), reinterpret_cast<char *>(&out));
        }
    }

    template<bool _signed, bool _escape, bool _dynamic_zip_int_type, std::uint16_t _header_type>
    friend class ZipInt;

public:
    ZipHelper()=delete;
    ~ZipHelper()=delete;
    ZipHelper(const ZipHelper &)=delete;
    ZipHelper(ZipHelper &&)=delete;
};

template <bool _signed, bool _escape, bool _dynamic_zip_int_type>
class ZipHelper::ZipData<_signed, _escape, _dynamic_zip_int_type, 0u> : public ZipDataDefault<0u> {

    // Attribute
    static constexpr std::uint16_t _ht = 0u;

    static constexpr bool hasLimit = _escape;
    static constexpr SizeType<_ht> limit = hasLimit ? 8 : std::numeric_limits<SizeType<_ht>>::max();

    // Data
    struct RefHeader {
        const uint8_t removeKey;
        const uint8_t addKey;
        const uint8_t signedRemoveKey;
        const uint8_t signedAddKey;

        constexpr RefHeader(uint8_t removeKey, uint8_t addKey, uint8_t signedKey)
            : removeKey(~removeKey), addKey(addKey), signedRemoveKey(~uint8_t(removeKey | signedKey)),
            signedAddKey(addKey | signedKey)
        {}
    };
    static constexpr RefHeader refHeader[8] {
        RefHeader{0b00000000, 0b00000000, 0b10000000},
        RefHeader{0b10000000, 0b00000000, 0b01000000},
        RefHeader{0b11000000, 0b10000000, 0b00100000},
        RefHeader{0b11100000, 0b11000000, 0b00010000},
        RefHeader{0b11110000, 0b11100000, 0b00001000},
        RefHeader{0b11111000, 0b11110000, 0b00000100},
        RefHeader{0b11111100, 0b11111000, 0b00000010},
        RefHeader{0b11111110, 0b11111100, 0b00000001}
    };
    // Member

    [[nodiscard]] SizeType<_ht> GetDataSize() const
    {
        return self.size;
    }

    void setHeader(uint8_t *data, bool isSigned) const
    { // This function shouldn't be called if that data is escaped
        SizeType<_ht> idx;

        for (idx = self.size; idx >= 8; idx -= 8, ++data)
            *data = 0b11111111;

        const auto &ref = ZipData::refHeader[idx];

        if constexpr (_signed) {
            *data &= ref.signedRemoveKey;
            *data |= isSigned ? ref.signedAddKey : ref.addKey;
        } else {
            *data &= ref.removeKey;
            *data |= ref.addKey;
        }
    }


public:
    explicit ZipData(const uint32_t nbOfUsedBits)
    {
        const uint32_t fullyUsedByte = nbOfUsedBits / 8;
        const uint16_t freeBits = 8 - (nbOfUsedBits % 8);

        self.size = 1 + fullyUsedByte + (fullyUsedByte / 8);
        self.size += ((self.size + _signed) > freeBits);

        if constexpr (ZipData::hasLimit) {
            self.size = (self.size >= ZipData::limit) ? 0 : self.size;
        }
    };

    friend ZipInt<_signed, _escape, _dynamic_zip_int_type, _ht>;
};

template<bool _signed, bool _escape, bool _dynamic_zip_int_type, std::uint16_t _header_type>
class ZipInt {
    static_assert(_header_type <= 1, "ZipInt _header_type can't be higher that 1");
    // Warning for the future if more _header_type were to be add, it can't be higher than 31!!

    using Zip = ZipInt<_signed, _escape, _dynamic_zip_int_type, _header_type>;
    using ZipData = ZipHelper::ZipData<_signed, _escape, _dynamic_zip_int_type, _header_type>;

    using SignedType = std::conditional_t<not _signed, const bool, bool>;
    using SizeType = ZipHelper::SizeType<_header_type>;


    uint16_t nbPower;               // 2B
    union  {
        uint8_t head;           // 1B
        char c_head{};          // 1B
        char ard_head[0];       // 0B
    };                              // 1B
    SignedType isSigned;            // 1B   -> should round up to 4B
    SizeType dataSize;              // 4B or 8B, if sizeof(SizeType) is 4B than round up to 8B else round up to 16B



// Member

    template<typename _stream, typename _tp>
    void _zip(_stream &stream, const _tp &data)
    {
        ZipHelper::bits<sizeof(_tp) * 8> enData;

        ZipHelper::_endianlessify(data, enData);

        if constexpr (_signed)
            self.isSigned = enData.get(0);

        if constexpr (_escape && std::is_same_v<Escape, _tp>) {
            // The data is an escape
            // Todo
            return;
        }

        // Find the number of useless bits
        SizeType nbOfUsedBits = (sizeof(_tp) * 8);
        for (uint32_t i = 0; enData.get(i) == self.isSigned && i < sizeof(_tp) * 8; ++i)
            --nbOfUsedBits;

        // Create a zipData, witch will compute the size of the data
        ZipData zipData(nbOfUsedBits);

        if constexpr (_escape) {
            if (zipData.size == 0) {
                // Todo better throw
                throw std::out_of_range("Too large number");
            }
        }

        // Create a buffer to store the new data
        uint8_t buffer[zipData.size];

        const uint8_t *itBegin = enData.cbyte_begin();
        uint8_t *itBuffer = buffer;

        if (zipData.size > sizeof(_tp)) {
            // If this is true it means, that more data will be used to serialize the data than the data actually weight

            itBuffer += zipData.size - sizeof(_tp);

            // File the start of the buffer than may never be initialized other wise
            if constexpr (_header_type == 0) {
                // Optimization for this case "_header_type == 0"
                itBuffer[-1] = 0b00000000;
            } else {
                std::fill(buffer, itBuffer, 0b00000000);
            }

        } else {
            SizeType diff = zipData.size - (nbOfUsedBits / 8) - 1;

            itBegin += sizeof(_tp) - diff - 1;
        }

        { // Debug
            std::cout << data << std::endl;

            for (uint32_t i = 0; i < sizeof(_tp) * 8; ++i) {
                if (i && !(i % 8))
                    std::cout << ':';
                std::cout << enData.get(i);
            }
            std::cout << std::endl;
        }

        if constexpr (_signed)
            if (self.isSigned)
                enData.set(0, false);

        // Copy the data
        std::copy(itBegin, enData.cbyte_end(), itBuffer);

        // Add the header
        zipData.setHeader(buffer, self.isSigned);

        { // DEBUG
            ZipHelper::bits<sizeof(_tp) * 8 + 100> &newData = *reinterpret_cast<ZipHelper::bits<
                    sizeof(_tp) * 8 + 100> *>(&buffer[0]);

            for (uint32_t i = 0; i < zipData.size * 8; ++i) {
                if (i && !(i % 8))
                    std::cout << ':';
                std::cout << newData.get(i);
            }
            std::cout << std::endl;
        }
    }


    template<typename _stream, bool _container>
    void _typeIt(_stream &stream)
    {
         {
            static_assert(_header_type <= 31, "ERROR FOR DEV, It seems like a _header_type superior"
                                              " to 31 has been created, sadly for binary capacity "
                                              "reason it can't happen, or just don't use dynamic");

            constexpr const uint8_t flag = (uint8_t(_signed) << uint8_t(7)) +
                                   (uint8_t(_escape) << uint8_t(6)) +
                                   (uint8_t(_container) << uint8_t(5)) +
                                   (uint8_t(_header_type));

            stream.write(&flag, 1);
        }
    }


    template<typename _stream, typename _tp, std::enable_if_t<ZipHelper::detail::is_iterable_v<_stream>, int> = 0>
    void _write(_stream &stream, const _tp &data)
    {
        // The data is a container

        if constexpr (_dynamic_zip_int_type) {
            self.template _typeIt<_stream, true>(stream);
            self._zip(stream, ZipHelper::detail::size(data));
        }
        for (const auto &obj : data)
            self._zip(stream, obj);
    }

    template<typename _stream, typename _tp>
    void _write(_stream &stream, const _tp &data)
    {
        if constexpr (_dynamic_zip_int_type)
            self.template _typeIt<_stream, false>(stream);
        self._zip(stream, data);
    }


public:
    ZipInt()
        : nbPower(0), head(), isSigned(false), dataSize(0)
    {}

    template<typename _stream, typename _tp>
    auto write(_stream &stream, const _tp &data)
        -> std::enable_if_t<ZipHelper::detail::has_write_v<_stream>>
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