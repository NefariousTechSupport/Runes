/*
	File:
		RfidTag.cpp

	Description:
		Class for interacting with figure data.
*/


#ifndef RUNES_RFID_TAG_H
#define RUNES_RFID_TAG_H

#include <cstdint>

#define NUM_BLOCKS 64
#define BLOCK_SIZE 16
#define TAG_SIZE (NUM_BLOCKS * BLOCK_SIZE)

namespace Runes
{
	// This class represents a figure's raw data, and provides an interface for
	// reading/writing to and from it, with support for different data regions
	// and encryption/decryption
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
			uint8_t DetermineActiveDataRegionInternal(uint8_t block0, uint8_t block1, uint8_t offset);
			uint8_t _tag[1024];
	};
}

#endif // RUNES_RFID_TAG_H