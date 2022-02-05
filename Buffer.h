#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <tuple>

namespace Buffer
{
    namespace
    {
        template<class, template<class...> class>
        inline constexpr bool isSpecialization = false;
        template<template<class...> class T, class... Args>
        inline constexpr bool isSpecialization<T<Args...>, T> = true;

        template<class T>
        concept isVector = isSpecialization<T, std::vector>;

        template<typename T>
        concept hasSize = requires(const T & t) { t.size(); };

        template<typename T>
        concept isTuple = requires (const T & t)
        {
            std::tuple_size<T>::value;
            std::get<0>(t);
        };

        template<typename T>
        concept isString = requires (const T & t) { std::common_type_t<T, std::string>(t); };
        template<typename T>
        concept isOneByteVariable = sizeof(T) == 1; // (std::same_as<T, uint8_t> || std::same_as<T, int8_t>);
        template<typename T>
        concept isTwoBytesVariable = sizeof(T) == 2; // (std::same_as<T, uint16_t> || std::same_as<T, int16_t>);
        template<typename T>
        concept isFourBytesVariable = sizeof(T) == 4; // (std::same_as<T, uint32_t> || std::same_as<T, int32_t>);
    }

    namespace From
    {
        template <typename T>
        static T Parse(const char* data)
        {
            if constexpr (isOneByteVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
                return static_cast<T>(*(d));
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
                return static_cast<T>(
                    (*(d + 1) << 8) |
                    (*(d)));
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
                return static_cast<T>(
                    (*(d + 3) << 24) |
                    (*(d + 2) << 16) |
                    (*(d + 1) << 8) |
                    (*(d)));
            }
            else if constexpr (isString<T>)
            {
                return std::string(data + 1, Parse<unsigned char>(data));
            }
            return T{};
        }
        
        template <typename T>
        static T internalParseVariable(const char* data, int* current)
        {
            // Note: no need to handle tuple, since std::tuple<..., std::tuple<...>, ...> does not make sense
            if constexpr (isVector<T>)
            {
                return internalParseVector<typename T::value_type>(data, current);
            }
            else if constexpr (isString<T>)
            {
                std::string message(Parse<std::string>(data + *current));
                *current += 1 + static_cast<char>(message.size());
                return message;
            }
            else if constexpr (isOneByteVariable<T>)
            {
                T val = Parse<T>(data + *current);
                *current += 1;
                return val;
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                T val = Parse<T>(data + *current);
                *current += 2;
                return val;
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                T val = Parse<T>(data + *current);
                *current += 4;
                return val;
            }

            return T{};
        }

        template <typename T>
        static std::vector<T> ParseVector(const char* data)
        {
            unsigned char size = Parse<unsigned char>(data);

            std::vector<T> output;
            output.reserve(size);

            int current = 1;
            for (unsigned char i = 0; i < size; i++) [[likely]]
                output.push_back(internalParseVariable<T>(data, &current));

            return output;
        }

        template <typename T>
        static std::vector<T> internalParseVector(const char* data, int* current)
        {
            unsigned char size = Parse<unsigned char>(data + *current);

            std::vector<T> output;
            output.reserve(size);

            *current += 1;
            for (unsigned char i = 0; i < size; i++) [[likely]]
                output.push_back(internalParseVariable<T>(data, current));

            return output;
        }

        template <typename ...Ts>
        static std::vector<std::tuple<Ts...>> ParseVectorOfTuple(const char* data)
        {
            unsigned char size = Parse<unsigned char>(data);

            std::vector<std::tuple<Ts...>> output;
            output.reserve(size);

            int current = 1;
            for (unsigned char i = 0; i < size; i++) [[likely]]
            {
                // Headaches. See: https://stackoverflow.com/a/14057064/10771848
                output.push_back({ Ts(internalParseVariable<Ts>(data, &current))... });
            }

            return output;
        }
    }

    namespace To
    {
        template<typename T>
        static char* Parse(T data)
        {
            if constexpr (isString<T>)
            {
                char* res = static_cast<char*>(malloc(1 + data.size()));
                res[0] = static_cast<char>(data.size());
                memcpy(res + 1, data.c_str(), data.size());
                return res;
            }
            else if constexpr (isOneByteVariable<T>)
            {
                char* res = static_cast<char*>(malloc(1));
                res[0] = static_cast<char>(data);
                return res;
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                char* res = static_cast<char*>(malloc(2));
                res[0] = (data & 0xFF);
                res[1] = (data >> 8 & 0xFF);
                return res;
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                char* res = static_cast<char*>(malloc(4));
                res[0] = (data & 0xFF);
                res[1] = (data >> 8 & 0xFF);
                res[2] = (data >> 16 & 0xFF);
                res[3] = (data >> 24 & 0xFF);
                return res;
            }

            char* res = static_cast<char*>(malloc(1));
            return res;
        }

        template<typename T>
        static char* internalParseOneByte(T data, char* buffer)
        {
            buffer[0] = static_cast<char>(data);
            return buffer;
        }

        template<typename T>
        static char* internalParseTwoBytes(T data, char* buffer)
        {
            buffer[0] = (data & 0xFF);
            buffer[1] = (data >> 8 & 0xFF);
            return buffer;
        }

        template<typename T>
        static char* internalParseFourBytes(T data, char* buffer)
        {
            buffer[0] = (data & 0xFF);
            buffer[1] = (data >> 8 & 0xFF);
            buffer[2] = (data >> 16 & 0xFF);
            buffer[3] = (data >> 24 & 0xFF);
            return buffer;
        }

        static char* internalParseString(const std::string& data, char* buffer)
        {
            buffer[0] = static_cast<char>(data.size());
            memcpy(buffer + 1, data.c_str(), data.size());
            return buffer;
        }

        template<typename T>
        size_t getSize(const T& t, bool isOwnedByVector = false)
        {
            if constexpr (isVector<T>)
            {
                size_t size = 1;
                for (const auto& elem : t) [[likely]]
                    size += getSize(elem, true);
                return size;
            }
            else if constexpr (isTuple<T>)
            {
                size_t size = 0;
                std::apply([&size, &isOwnedByVector](auto&&... args)
                {
                    ((size += getSize(args, isOwnedByVector)), ...);
                }, t);
                return size;
            }
            else if constexpr (hasSize<T>)
                return t.size() + isOwnedByVector;
            return sizeof t;
        }

        template<typename T>
        static void internalParseVariableToBuffer(const T& data, char* buffer, int* cursor)
        {
            if constexpr (isVector<T>)
            {
                buffer[*cursor] = static_cast<char>(data.size());
                *cursor += 1;
                for (const auto& d : data) [[likely]]
                    internalParseVariableToBuffer(d, buffer, cursor);
            }
            else if constexpr (isTuple<T>)
            {
                std::apply([&data, &buffer, &cursor](auto&&... args)
                    {
                        ((internalParseVariableToBuffer(args, buffer, cursor)), ...);
                    }, data);
            }
            else if constexpr (isString<T>)
            {
                internalParseString(data, buffer + *cursor);
                *cursor += 1 + static_cast<char>(data.size());
            }
            else if constexpr (isOneByteVariable<T>)
            {
                internalParseOneByte(data, buffer + *cursor);
                *cursor += 1;
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                internalParseTwoBytes(data, buffer + *cursor);
                *cursor += 2;
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                internalParseFourBytes(data, buffer + *cursor);
                *cursor += 4;
            }
        }

        template<typename T>
        static std::pair<char*, size_t> ParseVector(const std::vector<T>& data)
        {
            // Note: output is not null terminated and is not allocated of size + 1
            size_t size = getSize(data);
            char* output = static_cast<char*>(malloc(size));
            output[0] = static_cast<char>(data.size());
            int cursor = 1;

            for (auto const& d : data) [[likely]]
                internalParseVariableToBuffer(d, output, &cursor);

            return { output, size };
        }
    }

    static char* InitializeBuffer(int size)
    {
        if (size <= 0) [[unlikely]]
            return nullptr;
        char* buffer = static_cast<char*>(malloc(size));
        return buffer;
    }
}
