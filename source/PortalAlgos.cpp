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

	saltReady = true;
}