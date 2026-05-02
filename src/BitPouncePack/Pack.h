// BitPouncePack.h
#pragma once
#include "PackAsset.h"
#include <filesystem>
#include <vector>

namespace BitPouncePack {
    struct Pack {
        std::vector<PackAsset> assets;
    };

    Pack Load(const std::filesystem::path& filepath);
    void Save(const std::filesystem::path& filepath, const Pack& pack);
}