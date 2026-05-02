#include "BitPouncePack.h"
#include "BitPouncePackFileData.h"
#include "SerializationObject.h"
#include <fstream>
#include <cstring>
#include <vector>
#include <memory>
#include <stdexcept>

namespace BitPouncePack {

template<typename T>
void write_le(std::ostream& os, T value) {
    char* bytes = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i)
        os.put(bytes[i]);
}

template<typename T>
T read_le(std::istream& is) {
    T value = 0;
    char* bytes = reinterpret_cast<char*>(&value);
    is.read(bytes, sizeof(T));
    if (!is) throw std::runtime_error("Failed to read from file");
    return value;
}

uint64_t asset_size(const PackAsset& asset) {
    if (const auto* f = std::get_if<PackFile>(&asset)) return f->Size;
    if (const auto* t = std::get_if<PackTexture>(&asset)) return t->Size;
    if (const auto* s = std::get_if<PackScene>(&asset)) return s->Size;
    if (const auto* a = std::get_if<PackAudio>(&asset)) return a->Size;
    if (const auto* f = std::get_if<PackFont>(&asset)) return f->Size;
    return 0;
}

const std::byte* asset_data(const PackAsset& asset) {
    if (const auto* f = std::get_if<PackFile>(&asset)) return f->Data.get();
    if (const auto* t = std::get_if<PackTexture>(&asset)) return t->Data.get();
    if (const auto* a = std::get_if<PackAudio>(&asset)) return a->Data.get();
    if (const auto* f = std::get_if<PackFont>(&asset)) return f->FontData.get();
    return nullptr;
}

std::byte* asset_data(PackAsset& asset) {
    if (auto* f = std::get_if<PackFile>(&asset)) return f->Data.get();
    if (auto* t = std::get_if<PackTexture>(&asset)) return t->Data.get();
    if (auto* a = std::get_if<PackAudio>(&asset)) return a->Data.get();
    if (auto* f = std::get_if<PackFont>(&asset)) return f->FontData.get();
    return nullptr;
}

void write_metadata(std::ostream& os, const PackAsset& asset) {
    uint8_t type = asset.index();
    write_le(os, type);

    if (type == 0) { // PackFile
        const auto& f = std::get<PackFile>(asset);
        write_string(os, f.Name);
        write_le(os, f.hash.d1); write_le(os, f.hash.d2);
        write_le(os, f.hash.d3); write_le(os, f.hash.d4);
        write_le(os, f.Size);
        write_le(os, f.HashType);
    }
    else if (type == 1) { // PackTexture
        const auto& t = std::get<PackTexture>(asset);
        write_le(os, t.hash.d1); write_le(os, t.hash.d2);
        write_le(os, t.hash.d3); write_le(os, t.hash.d4);
        write_le(os, t.AssetHandle);
        write_le(os, t.Size);
        write_le(os, t.Width);
        write_le(os, t.Height);
        write_le(os, t.HashType);
        write_le(os, t.Filter);
        write_le(os, t.Channels);
    }
    else if (type == 2) { // PackScene
        const auto& s = std::get<PackScene>(asset);
        s.WriteMeta(os);
    }
    else if (type == 3) { // PackAudio
        const auto& a = std::get<PackAudio>(asset);
        write_le(os, a.hash.d1); write_le(os, a.hash.d2);
        write_le(os, a.hash.d3); write_le(os, a.hash.d4);
        write_le(os, a.AssetHandle);
        write_le(os, a.Size);
        write_le(os, a.Channels);
        write_le(os, a.SampleRate);
        write_le(os, a.HashType);
        write_le(os, static_cast<uint8_t>(a.format));
    }
    else if (type == 4) { // PackFont
        const auto& f = std::get<PackFont>(asset);
        write_le(os, f.hash.d1); write_le(os, f.hash.d2);
        write_le(os, f.hash.d3); write_le(os, f.hash.d4);
        write_le(os, f.AssetHandle);
        write_le(os, f.Size);
        write_le(os, f.Width);
        write_le(os, f.Height);
        write_le(os, f.HashType);
        write_le(os, f.Channels);
        write_le(os, f.EmSize);
        write_le(os, f.PixelRange);
        write_le(os, f.MiterLimit);
        write_le(os, f.FontScale);

        uint64_t glyphCount = f.Glyphs.size();
        write_le(os, glyphCount);
        for (const auto& g : f.Glyphs) {
            write_le(os, g.plane_l); write_le(os, g.plane_b);
            write_le(os, g.plane_r); write_le(os, g.plane_t);
            write_le(os, g.atlas_l); write_le(os, g.atlas_b);
            write_le(os, g.atlas_r); write_le(os, g.atlas_t);
            write_le(os, g.advance);
            write_le(os, g.codepoint);
        }

        // Write atlas bitmap as a binary blob (size + data)
        uint64_t atlasSize = f.Width * f.Height * f.Channels;
        write_le(os, atlasSize);
        os.write(reinterpret_cast<const char*>(f.AtlasData.get()), atlasSize);
    }
    else {
        throw std::runtime_error("Unknown asset type");
    }
}

PackAsset read_metadata(std::istream& is) {
    uint8_t type = read_le<uint8_t>(is);
    if (type == 0) {
        PackFile f;
        f.Name = read_string(is);
        f.hash.d1 = read_le<uint64_t>(is); f.hash.d2 = read_le<uint64_t>(is);
        f.hash.d3 = read_le<uint64_t>(is); f.hash.d4 = read_le<uint64_t>(is);
        f.Size = read_le<uint64_t>(is);
        f.HashType = read_le<uint8_t>(is);
        f.Data = nullptr;
        return f;
    }
    else if (type == 1) {
        PackTexture t;
        t.hash.d1 = read_le<uint64_t>(is); t.hash.d2 = read_le<uint64_t>(is);
        t.hash.d3 = read_le<uint64_t>(is); t.hash.d4 = read_le<uint64_t>(is);
        t.AssetHandle = read_le<uint64_t>(is);
        t.Size = read_le<uint64_t>(is);
        t.Width = read_le<uint32_t>(is);
        t.Height = read_le<uint32_t>(is);
        t.HashType = read_le<uint8_t>(is);
        t.Filter = read_le<uint8_t>(is);
        t.Channels = read_le<uint8_t>(is);
        t.Data = nullptr;
        return t;
    }
    else if (type == 2) {
        PackScene s;
        s.ReadMeta(is);
        return s;
    }
    else if (type == 3) {
        PackAudio a;
        a.hash.d1 = read_le<uint64_t>(is); a.hash.d2 = read_le<uint64_t>(is);
        a.hash.d3 = read_le<uint64_t>(is); a.hash.d4 = read_le<uint64_t>(is);
        a.AssetHandle = read_le<uint64_t>(is);
        a.Size = read_le<uint64_t>(is);
        a.Channels = read_le<uint32_t>(is);
        a.SampleRate = read_le<uint32_t>(is);
        a.HashType = read_le<uint8_t>(is);
        a.format = static_cast<AudioFormat>(read_le<uint8_t>(is));
        a.Data = nullptr;
        return a;
    }
    else if (type == 4) {
        PackFont f;
        f.hash.d1 = read_le<uint64_t>(is); f.hash.d2 = read_le<uint64_t>(is);
        f.hash.d3 = read_le<uint64_t>(is); f.hash.d4 = read_le<uint64_t>(is);
        f.AssetHandle = read_le<uint64_t>(is);
        f.Size = read_le<uint64_t>(is);
        f.Width = read_le<uint32_t>(is);
        f.Height = read_le<uint32_t>(is);
        f.HashType = read_le<uint8_t>(is);
        f.Channels = read_le<uint8_t>(is);
        f.EmSize = read_le<double>(is);
        f.PixelRange = read_le<double>(is);
        f.MiterLimit = read_le<double>(is);
        f.FontScale = read_le<double>(is);

        uint64_t glyphCount = read_le<uint64_t>(is);
        f.Glyphs.resize(glyphCount);
        for (auto& g : f.Glyphs) {
            g.plane_l = read_le<double>(is); g.plane_b = read_le<double>(is);
            g.plane_r = read_le<double>(is); g.plane_t = read_le<double>(is);
            g.atlas_l = read_le<double>(is); g.atlas_b = read_le<double>(is);
            g.atlas_r = read_le<double>(is); g.atlas_t = read_le<double>(is);
            g.advance = read_le<double>(is);
            g.codepoint = read_le<uint32_t>(is);
        }

        uint64_t atlasSize = read_le<uint64_t>(is);
        f.AtlasData = std::make_unique<std::byte[]>(atlasSize);
        is.read(reinterpret_cast<char*>(f.AtlasData.get()), atlasSize);
        if (!is) throw std::runtime_error("Failed to read font atlas data");

        f.FontData = nullptr;
        return f;
    }
    else {
        throw std::runtime_error("Invalid asset type in metadata");
    }
}

void Save(const std::filesystem::path& filepath, const Pack& pack) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot create pack file: " + filepath.string());

    uint64_t assetCount = pack.assets.size();

    BitPouncePackHeader header = {};
    std::memcpy(header.nub, "BPPACK", 6);
    header.ver = 1;
    header.features = 0;
    header.assetCount = assetCount;
    header.LookUpTablePos = sizeof(header);
    header.FileMetaDataPos = header.LookUpTablePos + assetCount * sizeof(FileOffset);

    file.write(header.nub, 6);
    write_le(file, header.ver);
    write_le(file, header.features);
    write_le(file, header.assetCount);
    write_le(file, header.LookUpTablePos);
    write_le(file, header.FileMetaDataPos);

    // Write metadata to temporary buffer
    std::ostringstream metaStream(std::ios::binary);
    for (size_t i = 0; i < assetCount; ++i) {
        write_metadata(metaStream, pack.assets[i]);
    }
    std::string metadata = metaStream.str();

    // Build lookup table (data offsets)
    std::vector<FileOffset> offsets(assetCount);
    uint64_t currentDataPos = header.FileMetaDataPos + metadata.size();
    for (size_t i = 0; i < assetCount; ++i) {
        offsets[i].begin = currentDataPos;
        offsets[i].end = currentDataPos + asset_size(pack.assets[i]);
        currentDataPos = offsets[i].end;
    }

    // Write lookup table
    file.seekp(header.LookUpTablePos);
    for (const auto& off : offsets) {
        write_le(file, off.begin);
        write_le(file, off.end);
    }

    // Write metadata block
    file.seekp(header.FileMetaDataPos);
    file.write(metadata.data(), metadata.size());

    // Write raw data blocks (font files, textures, audio, etc.)
    for (size_t i = 0; i < assetCount; ++i) {
        if (offsets[i].begin == offsets[i].end) continue;
        file.seekp(offsets[i].begin);
        const std::byte* data = asset_data(pack.assets[i]);
        if (!data) throw std::runtime_error("Asset has no data but non-zero size");
        file.write(reinterpret_cast<const char*>(data), asset_size(pack.assets[i]));
    }
}

Pack Load(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open pack file: " + filepath.string());
    BitPouncePackHeader header = {};
    file.read(header.nub, 6);
    header.ver = read_le<uint16_t>(file);
    header.features = read_le<uint64_t>(file);
    header.assetCount = read_le<uint64_t>(file);
    header.LookUpTablePos = read_le<uint64_t>(file);
    header.FileMetaDataPos = read_le<uint64_t>(file);

    if (std::memcmp(header.nub, "BPPACK", 6) != 0)
        throw std::runtime_error("Invalid signature");
    if (header.ver != 1)
        throw std::runtime_error("Unsupported version");

    uint64_t assetCount = header.assetCount;

    // Read lookup table
    std::vector<FileOffset> offsets(assetCount);
    file.seekg(header.LookUpTablePos);
    for (uint64_t i = 0; i < assetCount; ++i) {
        offsets[i].begin = read_le<uint64_t>(file);
        offsets[i].end   = read_le<uint64_t>(file);
    }

    // Read metadata
    std::vector<PackAsset> assets;
    assets.reserve(assetCount);
    file.seekg(header.FileMetaDataPos);
    for (uint64_t i = 0; i < assetCount; ++i) {
        assets.push_back(read_metadata(file));
    }

    // Read raw data blocks
    for (uint64_t i = 0; i < assetCount; ++i) {
        uint64_t size = offsets[i].end - offsets[i].begin;
        if (size == 0) continue;
        auto data = std::make_unique<std::byte[]>(size);
        file.seekg(offsets[i].begin);
        file.read(reinterpret_cast<char*>(data.get()), size);
        if (!file)
            throw std::runtime_error("Failed to read asset data");

        if (auto* f = std::get_if<PackFile>(&assets[i]))
            f->Data = std::move(data);
        else if (auto* t = std::get_if<PackTexture>(&assets[i]))
            t->Data = std::move(data);
        else if (auto* a = std::get_if<PackAudio>(&assets[i]))
            a->Data = std::move(data);
        else if (auto* f = std::get_if<PackFont>(&assets[i]))
            f->FontData = std::move(data);
    }

    Pack pack;
    pack.assets = std::move(assets);
    return pack;
}

} // namespace BitPouncePack