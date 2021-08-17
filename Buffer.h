#pragma once
#include <bit> // C++20, endianness
#include <string>

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
    {
        return static_cast<T>(
            (*(d + offset) << 8) |
            (*(d + offset + 1)));
    }
    else if constexpr (std::endian::native == std::endian::little)
    {
        return static_cast<T>(
            (*(d + offset + 1) << 8) |
            (*(d + offset)));
    }
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
    {
        return static_cast<T>(
            (*(d + offset) << 24) |
            (*(d + offset + 1) << 16) |
            (*(d + offset + 2) << 8) |
            (*(d + offset + 3)));
    }
    else if constexpr (std::endian::native == std::endian::little)
    {
        return static_cast<T>(
            (*(d + offset + 3) << 24) |
            (*(d + offset + 2) << 16) |
            (*(d + offset + 1) << 8) |
            (*(d + offset)));
    }
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

std::string ParseString(const char* data, int offset)
{
    std::string output;
    for (int i = 0; *(data + offset + i); i++)
        output += *(data + offset + i);
    return std::move(output);
}

std::string ParseStringWithSize(const char* data, int offset, int size)
{
    return std::move(std::string(data, size));
}