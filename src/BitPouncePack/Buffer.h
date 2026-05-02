#pragma once
#include <cstdlib>
#include <cstddef>
#include <cstdint>

namespace BitPouncePack
{
    struct Buffer
    {
        std::byte* Data = nullptr;
        uint64_t Size = 0;

        Buffer() = default;
        explicit Buffer(uint64_t size) { Allocate(size); }

        // No copies
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        // Move semantics
        Buffer(Buffer&& other) noexcept
            : Data(other.Data), Size(other.Size)
        {
            other.Data = nullptr;
            other.Size = 0;
        }

        Buffer& operator=(Buffer&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                Data = other.Data;
                Size = other.Size;
                other.Data = nullptr;
                other.Size = 0;
            }
            return *this;
        }

        ~Buffer() { Release(); }

        void Allocate(uint64_t size)
        {
            Release();
            if (size == 0) return;
            Data = static_cast<std::byte*>(malloc(size));
            Size = size;
        }

        void Release()
        {
            free(Data);
            Data = nullptr;
            Size = 0;
        }

        template<typename T>
        T* As() { return reinterpret_cast<T*>(Data); }

        template<typename T>
        const T* As() const { return reinterpret_cast<const T*>(Data); }

        explicit operator bool() const { return Data && Size > 0; }
    };
}