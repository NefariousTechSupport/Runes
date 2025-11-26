/*
	File:
		RfidTag.cpp

	Description:
		Class for interacting with figure data.
*/


#include "RfidTag.hpp"

#include <iostream>

#include "PortalAlgos.hpp"



//=============================================================================
// isAccessControlBlock: Determines whether this block is an access control
// block.
//=============================================================================
bool Runes::RfidTag::isAccessControlBlock(int blockId)
{
	return (blockId % 4) == 3;
}


//=============================================================================
// ReadFromFile: reads a tag from a file.
//=============================================================================
bool Runes::RfidTag::ReadFromFile(const char* path)
{
	FILE* f = fopen(path, "rb");
	int res = 0;

	if(!f) goto error;

	fseek(f, 0, SEEK_SET);
	res = fread(this->_tag, 0x01, 1024, f);

	if(res != 1024) goto error;

	fclose(f);
	return true;

	error:
	fclose(f);
	return false;
}


//=============================================================================
// SaveToFile: Saves a tag to a file.
//=============================================================================
bool Runes::RfidTag::SaveToFile(const char* path)
{
	FILE* f = fopen(path, "wb");
	int res = 0;

	if(!f) goto error;

	fseek(f, 0, SEEK_SET);
	res = fwrite(this->_tag, 0x01, 1024, f);

	if(res != 1024) goto error;

	fflush(f);
	fclose(f);
	return true;

	error:
	fclose(f);
	return false;
}


//=============================================================================
// shouldEncrypt: Determines whether a given block should be encrypted or not.
//=============================================================================
bool Runes::RfidTag::shouldEncrypt(uint8_t blockId)
{
	return blockId >= 8 && !Runes::RfidTag::isAccessControlBlock(blockId);
}


//=============================================================================
// decrypt: Decrypts the figure data.
//=============================================================================
void Runes::RfidTag::decrypt()
{
	uint8_t blockData[BLOCK_SIZE];
	for(int i = 8; i < NUM_BLOCKS; i++)
	{
		if(Runes::RfidTag::isAccessControlBlock(i)) continue;

		uint8_t* dst = this->_tag + i * BLOCK_SIZE;

		Runes::decryptBlock((Runes::PortalTagHeader*)this->_tag, dst, blockData, i);		
		memcpy(dst, blockData, BLOCK_SIZE);
	}
}


//=============================================================================
// CopyBlocks: Copy the blocks from the tag to a destination buffer.
//=============================================================================
bool Runes::RfidTag::CopyBlocks(void* dst, uint8_t blockId, uint8_t numBlocks)
{
	uint8_t currentBlock = blockId;
	uint8_t blocksRead = 0;
	while(blocksRead < numBlocks && currentBlock < NUM_BLOCKS)
	{
		if(Runes::RfidTag::isAccessControlBlock(currentBlock))
		{
			currentBlock++;
			continue;
		}

		if(Runes::RfidTag::shouldEncrypt(currentBlock) && !Runes::RfidTag::AllZero((uint8_t*)this->_tag + currentBlock * BLOCK_SIZE))
		{
			decryptBlock((Runes::PortalTagHeader*)this->_tag, this->_tag + currentBlock * BLOCK_SIZE, (uint8_t*)dst + blocksRead * BLOCK_SIZE, currentBlock);
		}
		else
		{
			memcpy((uint8_t*)dst + blocksRead * BLOCK_SIZE, this->_tag + currentBlock * BLOCK_SIZE, BLOCK_SIZE);
		}
		currentBlock++;
		blocksRead++;
	}
	return blocksRead == numBlocks;
}


//=============================================================================
// AllZero: Determine whether a given block is entirely zeroed out .
//=============================================================================
bool Runes::RfidTag::AllZero(uint8_t* block)
{
	return (*reinterpret_cast<uint64_t*>(block) | *reinterpret_cast<uint64_t*>(block + 8)) == 0;
}


//=============================================================================
// SaveBlocks: Saves data from a source buffer to the tag.
//=============================================================================
bool Runes::RfidTag::SaveBlocks(void* src, uint8_t blockId, uint8_t numBlocks)
{
	uint8_t currentBlock = blockId;
	uint8_t blocksWritten = 0;
	uint8_t* dst = this->_tag;
	while(blocksWritten < numBlocks && currentBlock < NUM_BLOCKS)
	{
		if(Runes::RfidTag::isAccessControlBlock(currentBlock))
		{
			currentBlock++;
			continue;
		}

		if(Runes::RfidTag::shouldEncrypt(currentBlock))
		{
			encryptBlock((Runes::PortalTagHeader*)this->_tag, (uint8_t*)src + blocksWritten * BLOCK_SIZE, dst + currentBlock * BLOCK_SIZE, currentBlock);
		}
		else
		{
			memcpy(dst + currentBlock * BLOCK_SIZE, (uint8_t*)src + blocksWritten * BLOCK_SIZE, BLOCK_SIZE);
		}
		currentBlock++;
		blocksWritten++;
	}
	return blocksWritten == numBlocks;
}


//=============================================================================
// DetermineActiveDataRegion0: Get the active data area for region 0.
//=============================================================================
uint8_t Runes::RfidTag::DetermineActiveDataRegion0()
{
	return DetermineActiveDataRegionInternal(0x08, 0x24, 0x09);
}


//=============================================================================
// DetermineActiveDataRegion1: Get the active data area for region 1.
//=============================================================================
uint8_t Runes::RfidTag::DetermineActiveDataRegion1()
{
	return DetermineActiveDataRegionInternal(0x11, 0x2D, 0x02);
}


//=============================================================================
// DetermineActiveDataRegionInternal: Get the active data region
//=============================================================================
uint8_t Runes::RfidTag::DetermineActiveDataRegionInternal(uint8_t block0, uint8_t block1, uint8_t offset)
{
	uint8_t areaSequences[2];
	uint8_t areaHeader[16];
	bool    areaPopulated[2];

	// Default to 0
	uint8_t activeRegion = 0;


	CopyBlocks(areaHeader, block0, 1);
	areaSequences[0] = areaHeader[offset];
	areaPopulated[0]  = !AllZero(areaHeader);

	CopyBlocks(areaHeader, block1, 1);
	areaSequences[1] = areaHeader[offset];
	areaPopulated[1]  = !AllZero(areaHeader);


	if (areaPopulated[0] && areaPopulated[1])
	{
		// If both areas are populated

		// Default to region 0 if we can't reason about it, ideally
		// we'd prompt the user, however the codebase should be redesigned
		// a bit to allow for user intervention.
		activeRegion = 0;

		// Prioritise the one with the higher area sequence, we need to
		// be a bit careful and handle overflows hence we're adding 1
		     if (static_cast<uint8_t>(areaSequences[0] + 1) == areaSequences[1]) activeRegion = 1;
		else if (static_cast<uint8_t>(areaSequences[1] + 1) == areaSequences[0]) activeRegion = 0;
	}
	else if (areaPopulated[0] && !areaPopulated[1])
	{
		// If area 0 is populated but not area 1
		activeRegion = 0;
	}
	else if (!areaPopulated[0] && areaPopulated[1])
	{
		// If area 1 is populated but not area 0
		activeRegion = 1;
	}
	else if (!areaPopulated[0] && !areaPopulated[1])
	{
		// If neither area is populated, set it to 1 so that
		// we write region 0
		activeRegion = 1;
	}


	return activeRegion;
}
