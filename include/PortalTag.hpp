#pragma once

#include <cstdint>

#include "ESkylandersGame.hpp"
#include "kTfbSpyroTag_DecoID.hpp"
#include "kTfbSpyroTag_ToyType.hpp"
#include "RfidTag.hpp"

// Documentation about figures:
//	Figures consist of 64 blocks of data, each block is 16 bytes
//	Every (4n + 3)th block is an access control block
//	The first two blocks make up the tag header
//	Blocks 


namespace Runes
{
	//Size should be 0x20
	struct PortalTagHeader
	{
		/* 0x00 */ uint32_t _serial;
		/* 0x04 */ uint8_t _bcc;
		/* 0x05 */ uint8_t _manufacturerData[0x0B];
		/* 0x10 */ kTfbSpyroTag_ToyType _toyType;
		/* 0x13 */ uint8_t _importantByte;		//MUST BE SET TO 0 OR THE GAME WILL REFUSE TO READ THIS TOY
		/* 0x14 */ uint32_t _tradingCardId1;
		/* 0x18 */ uint32_t _tradingCardId2;
		/* 0x1C */ uint16_t _subType;
		/* 0x1E */ uint16_t _headerChecksum;
	};
#pragma pack(push, 1)
	//Size should be 0x50
	struct PortalTagMagicMoment
	{
		/* 0x00 */ uint16_t _experience2011_low;
		/* 0x02 */ uint8_t _experience2011_high;
		/* 0x03 */ uint16_t _coinCount;
		/* 0x05 */ uint32_t _cumulativeTime;
		/* 0x09 */ uint8_t unk05[10];
		/* 0x13 */ uint8_t _platformUse1;
		/* 0x14 */ uint16_t _hatID2011;
		/* 0x16 */ uint8_t unk16;
		/* 0x17 */ uint8_t _platformUse2;
		/* 0x18 */ uint8_t unk18[50];
		/* 0x4A */ uint16_t _heroPoints;
		/* 0x4C */ uint8_t unk4C[3];
		/* 0x4F */ uint8_t _ownerCount;
		/* 0x50 */ uint8_t unk50[0x60];
	};
#pragma pack(pop)
	//Size should be 0x60
	struct PortalTagMagicMomentRemaining
	{

	};
	class PortalTag
	{
		public:
			PortalTagHeader _tagHeader;
			PortalTagMagicMoment _tagMagicMoment;
			RfidTag* _rfidTag;

			char _webCode[12];
			void StoreHeader();
			void StoreMagicMoment();
			static void DecodeSubtype(uint16_t varId, ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DecodeSubtype(ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DebugPrintHeader();
	};
}