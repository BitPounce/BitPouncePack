#include "PackScene.h"
#include "BitPouncePackFileData.h" // for write/read helpers

namespace BitPouncePack
{
    void PackScene::WriteMeta(std::ostream& os) const
    {
        write_string(os, Name);
        write_binary(os, AssetHandle);
        write_binary(os, hash.d1);
        write_binary(os, hash.d2);
        write_binary(os, hash.d3);
        write_binary(os, hash.d4);
        write_binary(os, Size);
        write_binary(os, HashType);
        Info.Write(os);
        Entities.Write(os);
    }

    void PackScene::ReadMeta(std::istream& is)
    {
        Name = read_string(is);
        AssetHandle = read_binary<uint64_t>(is);
        hash.d1 = read_binary<uint64_t>(is);
        hash.d2 = read_binary<uint64_t>(is);
        hash.d3 = read_binary<uint64_t>(is);
        hash.d4 = read_binary<uint64_t>(is);
        Size = read_binary<uint64_t>(is);
        HashType = read_binary<uint8_t>(is);
        Info.Read(is);
        Entities.Read(is);
    }
}