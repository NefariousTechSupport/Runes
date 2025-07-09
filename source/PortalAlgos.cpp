#include "PortalAlgos.hpp"
#include "3rd_party/md5.h"
#include "3rd_party/rijndael.h"

#include <memory>
#include <iostream>
#include <cstring>

uint8_t salt[0x35];
bool saltReady = false;

void Runes::computeKey(Runes::PortalTagHeader* header, uint8_t blockId, uint8_t key[0x10])
{
	uint8_t keyComponents[0x56];
	memcpy(keyComponents, header, 0x20);
	keyComponents[0x20] = blockId;

	if(!saltReady) readSalt();
	memcpy(keyComponents + 0x21, salt, 0x35);

	MD5* md5 = new MD5();
	MD5Open(md5);
	MD5Digest(md5, keyComponents, 0x56);
	MD5Close(md5, key);
}
void Runes::decryptBlock(Runes::PortalTagHeader* header, uint8_t* cBlockData, uint8_t* pBlockData, uint8_t blockId)
{
	uint8_t key[0x10];
	Runes::computeKey(header, blockId, key);

	unsigned long rk[RKLENGTH(128)];
	uint32_t rounds = rijndaelSetupDecrypt(rk, key, 128);
	rijndaelDecrypt(rk, rounds, cBlockData, pBlockData);
}
void Runes::encryptBlock(Runes::PortalTagHeader* header, uint8_t* pBlockData, uint8_t* cBlockData, uint8_t blockId)
{
	uint8_t key[0x10];
	Runes::computeKey(header, blockId, key);

	unsigned long rk[RKLENGTH(128)];
	uint32_t rounds = rijndaelSetupEncrypt(rk, key, 128);
	rijndaelEncrypt(rk, rounds, pBlockData, cBlockData);
}
std::optional<std::string> Runes::readSalt()
{
	if(saltReady) return std::nullopt;


	FILE* f = fopen("Resources/salt.txt", "rb");
	if(!f)
	{
		return "Failed to read Resources/salt.txt, you are meant to provide this file.\n";
	}

	fseek(f, 0, SEEK_END);
	if(ftell(f) != sizeof(salt))
	{
		return "Invalid salt.txt file provided.\n";
	}

	fseek(f, 0, SEEK_SET);
	int res = fread(salt, 0x01, sizeof(salt), f);
	if(res != sizeof(salt))
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "Failed reading salt.txt, received ferror %d.\n", ferror(f));
		return std::string(buf);
	}

	// Attempt to use this key on a block of data, assume this data is 0 when decrypted

	// compute the key..

	uint8_t keyCheck[0x56];
	memset(keyCheck, 0x00, sizeof(keyCheck));

	uint8_t keyHash[0x10];
	uint8_t cData[0x10] {0x58, 0xF4, 0xA7, 0x41, 0x86, 0x5D, 0xFB, 0xA2, 0x74, 0xF3, 0x3E, 0xE4, 0x52, 0x13, 0xD4, 0x39};
	keyCheck[0x20] = 8;
	memcpy(keyCheck + 0x21, salt, 0x35);

	MD5* md5 = new MD5();
	MD5Open(md5);
	MD5Digest(md5, keyCheck, 0x56);
	MD5Close(md5, keyHash);

	// decrypt the data...

	uint8_t pData[0x10];

	unsigned long rk[RKLENGTH(128)];
	uint32_t rounds = rijndaelSetupDecrypt(rk, keyHash, 128);
	rijndaelDecrypt(rk, rounds, cData, pData);

	// ensure the block is zeroed out
	bool zeroOut = (*(uint64_t*)&pData == 0) && (*(uint64_t*)&pData[8] == 0);

	if (!zeroOut)
	{
		return "Invalid salt.txt file provided";
	}

	saltReady = true;
	
	return std::nullopt;
}