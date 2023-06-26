#pragma once

#include <cstdint>

#define NUM_BLOCKS 64
#define BLOCK_SIZE 16
#define TAG_SIZE (NUM_BLOCKS * BLOCK_SIZE)

namespace Runes
{
	class RfidTag
	{
		public:
			static bool isAccessControlBlock(int blockId);
			bool ReadFromFile(const char* path);
			bool CopyBlocks(void* dst, uint8_t blockId, uint8_t numBlocks);
			void decrypt();
		private:
			uint8_t _tag[1024];
	};
}