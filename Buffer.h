#pragma once
#include <bit> // C++20, endianness
#include <string>
#include <cstring>

namespace Buffer
{
    template<typename T>
    T ParseOneByte(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        return static_cast<T>(*(d + offset));
    }

    template<typename T>
    T ParseTwoBytes(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        if constexpr (std::endian::native == std::endian::big)
            return ParseTwoBytesBigEndian<T>(data, offset);
        else if constexpr (std::endian::native == std::endian::little)
            return ParseTwoBytesLittleEndian<T>(data, offset);
    }

    template<typename T>
    T ParseTwoBytesLittleEndian(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        return static_cast<T>(
            (*(d + offset + 1) << 8) |
            (*(d + offset)));
    }

    template<typename T>
    T ParseTwoBytesBigEndian(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        return static_cast<T>(
            (*(d + offset) << 8) |
            (*(d + offset + 1)));
    }

    template<typename T>
    T ParseFourBytes(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        if constexpr (std::endian::native == std::endian::big)
            return ParseFourBytesBigEndian<T>(data, offset);
        else if constexpr (std::endian::native == std::endian::little)
            return ParseFourBytesLittleEndian<T>(data, offset);
    }

    template<typename T>
    T ParseFourBytesLittleEndian(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        return static_cast<T>(
            (*(d + offset + 3) << 24) |
            (*(d + offset + 2) << 16) |
            (*(d + offset + 1) << 8) |
            (*(d + offset)));
    }

    template<typename T>
    T ParseFourBytesBigEndian(const char* data, int offset)
    {
        const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
        return static_cast<T>(
            (*(d + offset) << 24) |
            (*(d + offset + 1) << 16) |
            (*(d + offset + 2) << 8) |
            (*(d + offset + 3)));
    }

    template<typename T>
    char* ParseOneByte(T data)
    {
        return reinterpret_cast<char*>(data);
    }

    template<typename T>
    char* ParseTwoBytes(T data)
    {
        if constexpr (std::endian::native == std::endian::big)
            return ParseTwoBytesBigEndian<T>(data);
        else if constexpr (std::endian::native == std::endian::little)
            return ParseTwoBytesLittleEndian<T>(data);
    }

    template<typename T>
    char* ParseTwoBytesLittleEndian(T data)
    {
        char* res = (char*)malloc(2);
        res[0] = (data & 0xFF);
        res[1] = (data >> 8 & 0xFF);
        return res;
    }

    template<typename T>
    char* ParseTwoBytesBigEndian(T data)
    {
        char* res = (char*)malloc(2);
        res[0] = (data >> 8 & 0xFF);
        res[1] = (data & 0xFF);
        return res;
    }

    template<typename T>
    char* ParseFourBytes(T data)
    {
        if constexpr (std::endian::native == std::endian::big)
            return ParseFourBytesBigEndian<T>(data);
        else if constexpr (std::endian::native == std::endian::little)
            return ParseFourBytesLittleEndian<T>(data);
    }

    template<typename T>
    char* ParseFourBytesLittleEndian(T data)
    {
        char* res = (char*)malloc(4);
        res[0] = (data & 0xFF);
        res[1] = (data >> 8 & 0xFF);
        res[2] = (data >> 16 & 0xFF);
        res[3] = (data >> 24 & 0xFF);
        return res;
    }

    template<typename T>
    char* ParseFourBytesBigEndian(T data)
    {
        char* res = (char*)malloc(4);
        res[0] = (data >> 24 & 0xFF);
        res[1] = (data >> 16 & 0xFF);
        res[2] = (data >> 8 & 0xFF);
        res[3] = (data & 0xFF);
        return res;
    }

    std::string ParseString(const char* data, int offset)
    {
        return std::move(std::string(data + offset));
    }

    std::string ParseStringWithSize(const char* data, int offset, int size)
    {
        return std::move(std::string(data + offset, size));
    }

    char* InitializeBuffer(int size)
    {
        if (size <= 0)
            return nullptr;
        char* buffer = (char*)malloc(size);
        if (buffer == nullptr)
            return nullptr;
        memset(buffer, 0, size);
        return buffer;
    }

    void Concat(char* prefix, const char* suffix, int size)
    {
        memcpy(prefix, suffix, size);
    }

    class Bufferizable
    {
    public:
        virtual char* Bufferize() { return nullptr; }
        virtual void Unbufferize(const char* Buffer) {}
    };
}
