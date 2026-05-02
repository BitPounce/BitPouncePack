#pragma once
#include <cstdint>

#pragma pack(push, 1)

namespace BitPouncePack
{
    struct BitPouncePackHeader
    {
        char nub[6];                // "BPPACK"
        uint16_t ver;               // current = 1
        uint64_t features;          // reserved for future flags
        uint64_t assetCount;        // number of assets in the pack
        uint64_t LookUpTablePos;    // file offset of lookup table
        uint64_t FileMetaDataPos;   // file offset of metadata block
    };

    struct FileOffset
    {
        uint64_t begin;
        uint64_t end;
    };
}

#pragma pack(pop)