#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include "Hash.h"

namespace BitPouncePack
{
	enum class AudioFormat : uint8_t
	{
		unknown = 0,
		u8      = 1,
		s16     = 2,
		s24     = 3,
		s32     = 4,
		f32     = 5,
		count
	};

	struct PackAudio
	{
		Hash hash;
		uint64_t AssetHandle;
		uint64_t Size;
		uint32_t Channels;
		uint32_t SampleRate;
		uint8_t HashType = 0;          // 0 = SHA-256
		AudioFormat format;

		std::unique_ptr<std::byte[]> Data;

		PackAudio() = default;
		PackAudio(const PackAudio&) = delete;
		PackAudio& operator=(const PackAudio&) = delete;
		PackAudio(PackAudio&&) = default;
		PackAudio& operator=(PackAudio&&) = default;
	};
}