#pragma once
#include <cstdint>
#include <memory>
#include "Hash.h"

namespace BitPouncePack
{
	struct PackTexture
	{
		Hash hash;
		uint64_t AssetHandle;
		uint64_t Size;
		uint32_t Width;
		uint32_t Height;
		// NEAREST = 0, LINEAR = 1
		uint8_t Filter;

		uint8_t HashType = 0;
		uint8_t Channels = 0;

		std::unique_ptr<std::byte[]> Data;

		PackTexture() = default;
		PackTexture(const PackTexture&) = delete;
		PackTexture& operator=(const PackTexture&) = delete;
		PackTexture(PackTexture&&) = default;
		PackTexture& operator=(PackTexture&&) = default;
	};
}