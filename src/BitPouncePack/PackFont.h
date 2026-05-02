#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "Hash.h"

namespace BitPouncePack
{
    struct GlyphMetrics
    {
        double plane_l, plane_b, plane_r, plane_t;
        double atlas_l, atlas_b, atlas_r, atlas_t;
        double advance;
        uint32_t codepoint;
    };

    struct PackFont
    {
        Hash hash;                      // SHA‑256 of raw font file
        uint64_t AssetHandle;
        uint64_t Size;                  // size of raw font file (TTF/OTF)
        uint32_t Width, Height;         // atlas dimensions
        uint8_t HashType = 0;
        uint8_t Channels = 4;           // RGBA

        std::unique_ptr<std::byte[]> FontData;   // raw font file (main data block)
        std::unique_ptr<std::byte[]> AtlasData;  // RGBA8 atlas bitmap (stored in metadata)

        std::vector<GlyphMetrics> Glyphs;
        double EmSize = 160.0;
        double PixelRange = 4.0;
        double MiterLimit = 1.0;
        double FontScale = 160.0;       // actual scale after packing

        PackFont() = default;
        PackFont(const PackFont&) = delete;
        PackFont& operator=(const PackFont&) = delete;
        PackFont(PackFont&&) = default;
        PackFont& operator=(PackFont&&) = default;
    };
}