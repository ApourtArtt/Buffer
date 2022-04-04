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

    class Buffer
    {
    public:
        template<typename ...Ts>
        Buffer(const Ts&... Args)
        {
            data = new char[getSize(Args...)];
            handleArg(Args...);
        }

        ~Buffer()
        {
            if (data != nullptr) [[likely]]
            {
                delete[] data;
                data = nullptr;
            }
        }

        template<typename T>
        static T GetArguments(const char* Data)
        {
            size_t Cursor = 0;
            return retrieveArg<T>(Data, Cursor);
        }

        template<typename T>
        static std::pair<T, size_t> GetArgumentsAndSize(const char* Data)
        {
            size_t Cursor = 0;
            return { retrieveArg<T>(Data, Cursor), Cursor };
        }

        [[nodiscard]] const size_t GetSize() const noexcept { return size; }
        [[nodiscard]] const char* GetData() const noexcept { return data; }
        [[nodiscard]] const std::string GetDataAsString() const { return std::string(data, size); }

    private:
        template<typename T>
        static T retrieveArg(const char* Data, size_t& Cursor)
        {
            if constexpr (isVector<T>)
            {
                unsigned char dataSize = static_cast<unsigned char>(*(Data + Cursor));
                T output;
                output.reserve(dataSize);
                Cursor += 1;
                for (unsigned char i = 0; i < dataSize; i++) [[likely]]
                {
                    output.push_back(retrieveArg<typename T::value_type>(Data, Cursor));
                }
                return output;
            }
            else if constexpr (isTuple<T>)
            {
                return [&Data, &Cursor] <typename... Ts>(std::type_identity<std::tuple<Ts...>>)
                {
                    // Headaches. See: https://stackoverflow.com/a/14057064/10771848, https://stackoverflow.com/questions/60169058/expand-a-tuple-type-into-a-variadic-template
                    return std::tuple<Ts...>{ Ts(retrieveArg<Ts>(Data, Cursor))... };
                }(std::type_identity<T>{});
            }
            else if constexpr (isString<T>)
            {
                unsigned char dataSize = static_cast<unsigned char>(*(Data + Cursor));
                Cursor += 1;
                std::string output(Data + Cursor, dataSize);
                Cursor += dataSize;
                return output;
            }
            else if constexpr (isOneByteVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(Data + Cursor);
                Cursor += 1;
                return static_cast<T>(*(d));
            }
            else if constexpr (isTwoBytesVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(Data + Cursor);
                Cursor += 2;
                return static_cast<T>(
                    (*(d + 1) << 8) |
                    (*(d)));
            }
            else if constexpr (isFourBytesVariable<T>)
            {
                const unsigned char* d = reinterpret_cast<const unsigned char*>(Data + Cursor);
                Cursor += 4;
                return static_cast<T>(
                    (*(d + 3) << 24) |
                    (*(d + 2) << 16) |
                    (*(d + 1) << 8) |
                    (*(d)));
            }
            return T{};
        }

        void handleArg() {}
        template <typename First, typename... Rest>
        void handleArg(const First& first, const Rest&... rest)
        {
            if constexpr (isVector<First>)
            {
                data[size++] = static_cast<char>(first.size());
                for (auto const& d : first) [[likely]]
                    handleArg(d);
            }
            else if constexpr (isTuple<First>)
            {
                std::apply([this](auto&&... args)
                {
                    (handleArg(args), ...);
                }, first);
            }
            else if constexpr (isString<First>)
            {
                unsigned char strSize = static_cast<unsigned char>(first.size());
                data[size++] = static_cast<char>(strSize);
                memcpy(data + size, first.c_str(), first.size());
                size += strSize;
            }
            else if constexpr (isOneByteVariable<First>)
            {
                data[size++] = static_cast<char>(first);
            }
            else if constexpr (isTwoBytesVariable<First>)
            {
                data[size++] = static_cast<char>(first & 0xFF);
                data[size++] = static_cast<char>(first >> 8 & 0xFF);
            }
            else if constexpr (isFourBytesVariable<First>)
            {
                data[size++] = static_cast<char>(first & 0xFF);
                data[size++] = static_cast<char>(first >> 8 & 0xFF);
                data[size++] = static_cast<char>(first >> 16 & 0xFF);
                data[size++] = static_cast<char>(first >> 24 & 0xFF);
            }
            handleArg(rest...);
        }

        template<typename T>
        size_t getSizeSimple(const T& t)
        {
            if constexpr (isVector<T>)
            {
                size_t dataSize = 1;
                for (const auto& elem : t) [[likely]]
                    dataSize += getSizeSimple(elem);
                return dataSize;
            }
            else if constexpr (isTuple<T>)
            {
                size_t dataSize = 0;
                std::apply([&dataSize, this](auto&&... args)
                {
                    ((dataSize += getSizeSimple(args)), ...);
                }, t);
                return dataSize;
            }
            else if constexpr (hasSize<T>)
                return 1 + t.size();
            return sizeof t;
        }

        template <typename... Ts>
        size_t getSize(Ts&... args)
        {
            size_t size = 0;
            ((size += getSizeSimple(args)), ...);
            return size;
        }

        size_t size{ 0 };
        char* data{ nullptr }; // NOTE: not null terminated
    };
}
