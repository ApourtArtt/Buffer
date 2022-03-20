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
        static std::pair<std::vector<T>, int> ParseVectorGetSize(const char* data)
        {
            unsigned char size = Parse<unsigned char>(data);

            std::vector<T> output;
            output.reserve(size);

            int current = 1;
            for (unsigned char i = 0; i < size; i++) [[likely]]
                output.push_back(internalParseVariable<T>(data, &current));

            return { output, current };
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
        static void internalParseOneByte(T data, std::string& buffer)
        {
            buffer += static_cast<char>(data);
        }

        template<typename T>
        static void internalParseTwoBytes(T data, std::string& buffer)
        {
            buffer += (data & 0xFF);
            buffer += (data >> 8 & 0xFF);
        }

        template<typename T>
        static void internalParseFourBytes(T data, std::string& buffer)
        {
            buffer += (data & 0xFF);
            buffer += (data >> 8 & 0xFF);
            buffer += (data >> 16 & 0xFF);
            buffer += (data >> 24 & 0xFF);
        }

        static void internalParseString(const std::string& data, std::string& buffer)
        {
            buffer += static_cast<char>(data.size());
            buffer += data;
        }

        template<typename T>
        static std::string Parse(T data)
        {
            if constexpr (isString<T>)
            {
                std::string res;
                res.reserve(1 + data.size());
                internalParseString(data, res);
                return res;
            }
            else if constexpr (isOneByteVariable<T>)
            {
                std::string res;
                res.reserve(1);
                internalParseOneByte(data, res);
                return res;
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                std::string res;
                res.reserve(2);
                internalParseTwoBytes(data, res);
                return res;
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                std::string res;
                res.reserve(4);
                internalParseFourBytes(data, res);
                return res;
            }
            return std::string();
        }

        template<typename T>
        size_t GetSize(const T& t, bool isOwnedByVector = false)
        {
            if constexpr (isVector<T>)
            {
                size_t size = 1;
                for (const auto& elem : t) [[likely]]
                    size += GetSize(elem, true);
                return size;
            }
            else if constexpr (isTuple<T>)
            {
                size_t size = 0;
                std::apply([&size, &isOwnedByVector](auto&&... args)
                {
                    ((size += GetSize(args, isOwnedByVector)), ...);
                }, t);
                return size;
            }
            else if constexpr (hasSize<T>)
                return t.size() + isOwnedByVector;
            return sizeof t;
        }

        template<typename T>
        static void internalParseVariableToBuffer(const T& data, std::string& buffer)
        {
            if constexpr (isVector<T>)
            {
                buffer += static_cast<char>(data.size());
                for (const auto& d : data) [[likely]]
                    internalParseVariableToBuffer(d, buffer);
            }
            else if constexpr (isTuple<T>)
            {
                std::apply([&data, &buffer](auto&&... args)
                {
                    ((internalParseVariableToBuffer(args, buffer)), ...);
                }, data);
            }
            else if constexpr (isString<T>)
            {
                internalParseString(data, buffer);
            }
            else if constexpr (isOneByteVariable<T>)
            {
                internalParseOneByte(data, buffer);
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                internalParseTwoBytes(data, buffer);
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                internalParseFourBytes(data, buffer);
            }
        }

        template<typename T>
        static std::string ParseVector(const std::vector<T>& data)
        {
            // Note: output is not null terminated and is not allocated of size + 1
            std::string output;
            output.reserve(GetSize(data));
            output += static_cast<char>(data.size());

            for (auto const& d : data) [[likely]]
                internalParseVariableToBuffer(d, output);

            return output;
        }
    }
}
