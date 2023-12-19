#include "RfidTag.hpp"
#include <iostream>

#include "PortalAlgos.hpp"

bool Runes::RfidTag::isAccessControlBlock(int blockId)
{
	return (blockId % 4) == 3;
}
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
bool Runes::RfidTag::shouldEncrypt(uint8_t blockId)
{
	return blockId >= 8 && !Runes::RfidTag::isAccessControlBlock(blockId);
}
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
bool Runes::RfidTag::AllZero(uint8_t* block)
{
	return (*(uint64_t*)block + *((uint64_t*)(block + 8))) == 0;
}
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
uint8_t Runes::RfidTag::DetermineActiveDataRegion()
{
	uint8_t areaSequences[2];
	uint8_t areaHeader[16];
	CopyBlocks(areaHeader, 0x08, 1);
	areaSequences[0] = areaHeader[9];
	CopyBlocks(areaHeader, 0x24, 1);
	areaSequences[1] = areaHeader[9];
	if((areaSequences[0] + 1) == areaSequences[1]) return 1;	//Use area 1
	if((areaSequences[1] + 1) == areaSequences[0]) return 0;	//Use area 0
	return -1;
}