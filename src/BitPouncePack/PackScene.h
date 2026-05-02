#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "Hash.h"
#include "SerializationObject.h"

namespace BitPouncePack
{
	struct PackScene
	{
		std::string Name;
		uint64_t AssetHandle;
		Hash hash;
		uint64_t Size;
		uint8_t HashType = 0;

		SerializationObject Info;
		SerializationObjectArray Entities;

		// For binary serialization of the metadata (not the raw data – PackScene has no raw data)
		void WriteMeta(std::ostream& os) const;
		void ReadMeta(std::istream& is);
	};
}