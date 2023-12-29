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
			static bool shouldEncrypt(uint8_t blockId);
			static bool isAccessControlBlock(int blockId);
			static bool AllZero(uint8_t* block);
			bool ReadFromFile(const char* path);
			bool SaveToFile(const char* path);
			bool CopyBlocks(void* dst, uint8_t blockId, uint8_t numBlocks);
			bool SaveBlocks(void* src, uint8_t blockId, uint8_t numBlocks);
			void decrypt();
			uint8_t DetermineActiveDataRegion0();
			uint8_t DetermineActiveDataRegion1();
		private:
			uint8_t _tag[1024];
	};
}