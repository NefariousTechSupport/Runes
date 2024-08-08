#include "PortalAlgos.hpp"
#include "3rd_party/md5.h"
#include "3rd_party/rijndael.h"

#include <memory>
#include <iostream>

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
void Runes::readSalt()
{
	if(saltReady) return;


	FILE* f = fopen("Resources/salt.txt", "rb");
	if(!f)
	{
		printf("Failed to read Resources/salt.txt, you are meant to provide this file.\n");
		exit(-1);
		return;
	}

	fseek(f, 0, SEEK_END);
	if(ftell(f) != sizeof(salt))
	{
		printf("Invalid salt.txt file provided.\n");
		exit(-1);
		return;
	}

	fseek(f, 0, SEEK_SET);
	int res = fread(salt, 0x01, sizeof(salt), f);
	if(res != sizeof(salt))
	{
		printf("Failed reading salt.txt, received ferror %d.\n", ferror(f));
		exit(-1);
		return;
	}

	//This is probably shit but don't hate me it's my first time writing C++ and my IDE isn't set up :)
	//What this "attempts" to do is decrypt some example cipher data. If the salt provided is correct, then the outputted plain data will be full zero'd
	//Just to prevent correct-length-incorrect-salt files without the salt being possible to derive from the source code
	//- Texthead
	uint8_t keyCheck[0x56];
	uint8_t keyHash[0x10];
	uint8_t cData[0x10] { 88, 244, 167, 65, 134, 93, 251, 162, 116, 243, 62, 228, 82, 19, 212, 57 };
	keyCheck[0x20] = 8;
	memcpy(keyCheck + 0x21, salt, 0x35);

	MD5* md5 = new MD5();
	MD5Open(md5);
	MD5Digest(md5, keyCheck, 0x56);
	MD5Close(md5, keyHash);

	uint8_t pData[0x10];

	unsigned long rk[RKLENGTH(128)];
	uint32_t rounds = rijndaelSetupDecrypt(rk, keyHash, 128);
	rijndaelDecrypt(rk, rounds, cData, pData);

	bool zeroOut = true;
    for (size_t i = 0; i < sizeof(pData); ++i)
	{
        if (pData[i] != 0)
		{
            zeroOut = false;
            break;
        }
    }

	if (!zeroOut)
	{
		printf("Invalid salt.txt file provided");
		exit(-1);
		return;
	}

	saltReady = true;
}