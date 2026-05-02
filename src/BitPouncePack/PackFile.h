#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "Hash.h"

namespace BitPouncePack
{
    struct PackFile
    {
        std::string Name;
        Hash hash;
        uint64_t Size;
        uint8_t HashType = 0;          // 0 = SHA-256

        std::unique_ptr<std::byte[]> Data;

        PackFile() = default;
        PackFile(const PackFile&) = delete;
        PackFile& operator=(const PackFile&) = delete;
        PackFile(PackFile&&) = default;
        PackFile& operator=(PackFile&&) = default;
    };
}